

#ifndef NVISION_TEST_CORNER_GENERATOR_H_
#define NVISION_TEST_CORNER_GENERATOR_H_

#include <nvision/core.h>
#include <nvision/draw.h>

namespace nvision::test
{
    template<typename Scalar>
    struct CornerDefinition
    {
        using Vector2 = Eigen::Matrix<Scalar, 2, 1>;

        Vector2 topLeft = Vector2{0, 0};
        Scalar scale = 1;
        Scalar rotation = 0;
    };

    template<typename Scalar>
    class CornerGenerator
    {
    public:
        using FeatureMatrix = Eigen::Matrix<Scalar, 2, Eigen::Dynamic>;
        using Vector2 = Eigen::Matrix<Scalar, 2, 1>;
        using CornerVector2 = Eigen::Matrix<nvision::float32, 2, 1>;
        using Matrix2 = Eigen::Matrix<Scalar, 2, 2>;

        CornerGenerator(const std::vector<CornerDefinition<Scalar>> &definitions)
            : _definitions(definitions)
        {}

        template<typename Derived, typename ColorSpace>
        void generate(ImageBase<Derived> &img, FeatureMatrix &features, const Color<ColorSpace> &color) const
        {
            static_assert(IsImage<ImageBase<Derived>>::value, "image must be image type");

            features.resize(2, _definitions.size() * 3);

            for(size_t i = 0; i < _definitions.size(); ++i)
            {
                const auto &def = _definitions[i];
                auto cornerA = Vector2(0, 0);
                auto cornerB = Vector2(def.scale, 0);
                auto cornerC = Vector2(def.scale / 2, def.scale);

                const auto rotradian = (def.rotation / 180) * pi<Scalar>();

                const auto c = std::cos(rotradian);
                const auto s = std::sin(rotradian);
                auto rotation = Matrix2{};

                rotation << c, -s,
                            s,  c;

                cornerA = rotation * cornerA + def.topLeft;
                cornerB = rotation * cornerB + def.topLeft;
                cornerC = rotation * cornerC + def.topLeft;

                features.col(i * 3) = cornerA;
                features.col(i * 3 + 1) = cornerB;
                features.col(i * 3 + 2) = cornerC;


                auto poly = Polygon{};
                poly.points.push_back(cornerA.template cast<nvision::float32>());
                poly.points.push_back(cornerB.template cast<nvision::float32>());
                poly.points.push_back(cornerC.template cast<nvision::float32>());
                poly.fillMode = FillMode::Fill;

                image::draw(img, poly, color);
            }
        }

        const std::vector<CornerDefinition<Scalar>> &definitions() const
        {
            return _definitions;
        }

    private:
        std::vector<CornerDefinition<Scalar>> _definitions = {};
    };
}

#endif
