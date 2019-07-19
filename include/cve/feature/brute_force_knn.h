/* brute_force_knn.h
 *
 * Author: Fabian Meyer
 * Created On: 21 Jun 2019
 */

#ifndef CVE_BRUTE_FORCE_KNN_H_
#define CVE_BRUTE_FORCE_KNN_H_

#include <stdexcept>
#include "cve/core/matrix.h"

namespace cve
{
    template<typename Scalar>
    struct HammingDistance
    {
        template<typename Derived>
        Scalar operator()(const Eigen::MatrixBase<Derived> &lhs,
            const Eigen::MatrixBase<Derived> &rhs) const
        {
            Scalar dist = 0;
            for(Index i = 0; i < lhs.size(); ++i)
            {
                for(Index val = lhs(i) ^ rhs(i); val > 0; val >>= 1)
                {
                    if((val & 1) == 1)
                        dist += 1;
                }
            }

            return dist;
        }
    };

    template<typename Scalar>
    struct ManhattenDistance
    {
        template<typename Derived>
        Scalar operator()(const Eigen::MatrixBase<Derived> &lhs,
            const Eigen::MatrixBase<Derived> &rhs) const
        {
            return (lhs - rhs).array().abs().matrix().sum();
        }
    };

    template<typename Scalar>
    struct EuclideanDistance
    {
        template<typename Derived>
        Scalar operator()(const Eigen::MatrixBase<Derived> &lhs,
            const Eigen::MatrixBase<Derived> &rhs) const
        {
            return (lhs - rhs).norm();
        }
    };

    template<typename Scalar, typename Distance, typename DescScalar=Scalar>
    class BruteForceKNN
    {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Vector;
        typedef Eigen::Matrix<DescScalar, Eigen::Dynamic, Eigen::Dynamic> MatrixD;
        typedef Eigen::Matrix<DescScalar, Eigen::Dynamic, Eigen::Dynamic> VectorD;
    private:
        Distance distance_;
        const MatrixD *dataDescriptors_;
        MatrixD dataCopy_;

        Index threads_;
        bool sorted_;

        class QueryHeap
        {
        private:
            Index *indices_;
            Scalar *distances_;
            Index maxSize_;
            Index size_;
        public:
            QueryHeap(Index *indices, Scalar *distances, const Index maxSize)
                : indices_(indices), distances_(distances), maxSize_(maxSize),
                size_(0)
            {
            }

            void push(const Index idx, const Scalar dist)
            {
                if(full())
                    throw std::runtime_error("heap is full");
                // add new value at the end
                indices_[size_] = idx;
                distances_[size_] = dist;
                ++size_;

                // upheap
                Index k = size_ - 1;
                while(k > 0 && distances_[(k - 1) / 2] < dist)
                {
                    Index tmp = (k - 1) / 2;
                    distances_[k] = distances_[tmp];
                    indices_[k] = indices_[tmp];
                    k = tmp;
                }
                distances_[k] = dist;
                indices_[k] = idx;
            }

            void pop()
            {
                if(empty())
                    throw std::runtime_error("heap is empty");
                // replace first element with last
                distances_[0] = distances_[size_-1];
                indices_[0] = indices_[size_-1];
                --size_;

                // downheap
                Index k = 0;
                Scalar dist = distances_[0];
                Index idx = indices_[0];
                while(2 * k + 1 < size_)
                {
                    Index j = 2 * k + 1;
                    if(j + 1 < size_ && distances_[j+1] > distances_[j])
                        ++j;
                    // j references now greates child
                    if(dist >= distances_[j])
                        break;
                    distances_[k] = distances_[j];
                    indices_[k] = indices_[j];
                    k = j;
                }
                distances_[k] = dist;
                indices_[k] = idx;
            }

            Scalar front() const
            {
                if(empty())
                    throw std::runtime_error("heap is empty");

                return distances_[0];
            }

            bool full() const
            {
                return size_ >= maxSize_;
            }

            bool empty() const
            {
                return size_ == 0;
            }

            Index size() const
            {
                return size_;
            }

            void clear()
            {
                size_ = 0;
            }

            void sort()
            {
                Index cnt = size_;
                for(Index i = 0; i < cnt; ++i)
                {
                    Index idx = indices_[0];
                    Scalar dist = distances_[0];
                    pop();
                    indices_[cnt - i - 1] = idx;
                    distances_[cnt - i - 1] = dist;
                }
            }
        };
    public:

        BruteForceKNN()
            : distance_(), dataDescriptors_(nullptr), dataCopy_(), threads_(1),
            sorted_(true)
        { }

        BruteForceKNN(const MatrixD &dataDescriptors, const bool copy = false)
            : distance_(), dataDescriptors_(nullptr), dataCopy_(), threads_(1),
            sorted_(true)
        {
            setData(dataDescriptors, copy);
        }

        void setData(const MatrixD &dataDescriptors, const bool copy = false)
        {
            if(copy)
            {
                dataCopy_ = dataDescriptors;
                dataDescriptors_ = &dataCopy_;
            }
            else
            {
                dataDescriptors_ = &dataDescriptors;
            }
        }

        void setDistanceFunctor(const Distance &distance)
        {
            distance_ = distance;
        }

        void setThreads(const Index threads)
        {
            threads_ = threads;
        }

        void setSorted(const bool sorted)
        {
            sorted_ = sorted;
        }

        Index dimension() const
        {
            return dataDescriptors_->rows();
        }

        Index size() const
        {
            return dataDescriptors_->cols();
        }

        void query(const MatrixD &queryDescriptors,
            const Index knn,
            Matrixi &indices,
            Matrix &distances) const
        {
            if(dataDescriptors_ == nullptr)
                throw std::runtime_error("BruteForceKNN: data not set");
            if(dataDescriptors_->size() == 0)
                throw std::runtime_error("BruteForceKNN: data is empty");
            if(queryDescriptors.rows() != dimension())
                throw std::runtime_error("BruteForceKNN: data and query descriptors do not have same dimension");

            const MatrixD &dataDescriptors = *dataDescriptors_;

            indices.setConstant(knn, queryDescriptors.cols(), -1);
            distances.setConstant(knn, queryDescriptors.cols(), -1);

            // #pragma omp parallel for num_threads(threads_)
            for(Index i = 0; i < queryDescriptors.cols(); ++i)
            {
                Index *indicesPtr = &indices.data()[i * knn];
                Scalar *distancesPtr = &distances.data()[i * knn];
                QueryHeap heap(indicesPtr, distancesPtr, knn);

                for(Index j = 0; j < dataDescriptors.cols(); ++j)
                {
                    Scalar dist = distance_(queryDescriptors.col(i),
                        dataDescriptors.col(j));

                    if(!heap.full() || dist < heap.front())
                    {
                        if(heap.full())
                            heap.pop();
                        heap.push(j, dist);
                    }
                }

                if(sorted_)
                    heap.sort();
            }
        }
    };
}

#endif
