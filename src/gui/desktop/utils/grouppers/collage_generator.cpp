
#include <QPainter>
#include <core/oriented_image.hpp>

#include "collage_generator.hpp"

// materials:
// https://stackoverflow.com/questions/8535633/photo-collage-algorithm
// https://www.researchgate.net/profile/Kiyoharu-Aizawa/publication/269455490_Very_fast_generation_of_content-preserved_photo_collage_under_canvas_size_constraint/links/55d95b1008aec156b9ac35dc/Very-fast-generation-of-content-preserved-photo-collage-under-canvas-size-constraint.pdf?origin=publication_detail
// https://github.com/n-gao/collage-generator/blob/master/collage_generator.py

namespace
{
    struct Image
    {
        double a = 1.0;
        int idx = -1;

        Image(int w, int h, int i)
            : a (static_cast<double>(w)/h)
            , idx(i)
        {

        }
    };

    struct Node
    {
        double a_tar = 1.0;
        double a = 1.0;
        bool horizontalSplit = false;

        std::optional<Image> image;

        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        bool isLeaf() const
        {
            return image.has_value();
        }
    };

    std::pair<Image, Image> takeImgPair(std::vector<Image>& images, double a)
    {
        assert(std::is_sorted(images.begin(), images.end(), [](const Image& lhs, const Image& rhs)
        {
            return lhs.a < rhs.a;
        }));

        int p = 0;
        int q = images.size() - 1;
        int i = p;
        int j = q;
        double m = std::abs(images[p].a + images[q].a - a);

        while (p < q)
        {
            const double a_sum = images[p].a + images[q].a;

            if (a_sum > a)
            {
                if (std::abs(a_sum - a) < m)
                {
                    m = std::abs(a_sum - a);
                    i = p;
                    j = q;
                }
                --q;
            }
            else if (a_sum < a)
            {
                if (std::abs(a_sum - a) < m)
                {
                    m = std::abs(a_sum - a);
                    i = p;
                    j = q;
                }
                p++;
            }
            else
            {
                i = p;
                j = q;
                break;
            }
        }

        Image l = images[i];
        Image r = images[j];

        // remove from back first
        images.erase(images.begin() + j);
        images.erase(images.begin() + i);

        return std::make_pair(l, r);
    }


    Image takeImage(std::vector<Image>& images, double a)
    {
        auto best = std::min_element(images.begin(), images.end(), [a](const Image& lhs, const Image& rhs)
        {
            return std::abs(lhs.a - a) < std::abs(rhs.a - a);
        });

        Image image = *best;
        images.erase(best);

        return image;
    }

    std::unique_ptr<Node> generateTree(std::vector<Image>& images, int leafs, double ratio)
    {
        std::unique_ptr<Node> node = std::make_unique<Node>();
        node->a_tar = ratio;

        if (leafs == 1)
        {
            node->image = takeImage(images, ratio);
        }
        else
        {
            node->horizontalSplit = rand() % 2 == 0;

            const double a_tar_child = node->horizontalSplit? node->a_tar * 2: node->a_tar / 2;

            if (leafs > 2)
            {
                const int div = leafs/2;
                auto left = generateTree(images, div, a_tar_child);
                auto right = generateTree(images, leafs - div, a_tar_child);

                node->left = std::move(left);
                node->right = std::move(right);
            }
            else
            {
                const std::pair imagePair = takeImgPair(images, node->a_tar);

                auto left = std::make_unique<Node>();
                auto right = std::make_unique<Node>();

                left->image = imagePair.first;
                right->image = imagePair.second;

                node->left = std::move(left);
                node->right = std::move(right);
            }
        }

        return node;
    }

    double recurCalcAR(Node* n)
    {
        if (n->isLeaf())
            n->a = n->a_tar = n->image->a;
        else
        {
            double left_a = recurCalcAR(n->left.get());
            double right_a = recurCalcAR(n->right.get());

            if (n->horizontalSplit)
                n->a = left_a * right_a / (left_a + right_a);
            else
                n->a = left_a + right_a;
        }

        return n->a;
    }

    void adjustTree(Node* n, double th)
    {
        if (n->isLeaf() == false)
        {
            if (n->a > n->a_tar * th)
                n->horizontalSplit = true;

            if (n->a < n->a_tar / th)
                n->horizontalSplit = false;

            if (n->horizontalSplit)
            {
                n->left->a_tar = n->a_tar * 2;
                n->right->a_tar = n->a_tar * 2;
            }
            else
            {
                n->left->a_tar = n->a_tar / 2;
                n->right->a_tar = n->a_tar / 2;
            }

            adjustTree(n->left.get(), th);
            adjustTree(n->right.get(), th);
        }
    }

    void calculatePositionsForImages(std::vector<QRect>& positions, Node* n, QRect available_area)
    {
        if (n->isLeaf())
        {
            const int idx = n->image->idx;
            if (positions.size() < idx + 1)
                positions.resize(idx + 1);

            positions[idx] = available_area;
        }
        else
        {
            if (n->horizontalSplit)
            {
                const int l_height = available_area.width() / n->left->a;
                const QRect l_rect(available_area.x(), available_area.y(), available_area.width(), l_height);
                const QRect r_rect(available_area.x(), available_area.y() + l_height, available_area.width(), available_area.height() - l_height);

                calculatePositionsForImages(positions, n->left.get(), l_rect);
                calculatePositionsForImages(positions, n->right.get(), r_rect);
            }
            else
            {
                const int l_width = available_area.height() * n->left->a;
                const QRect l_rect(available_area.x(), available_area.y(), l_width, available_area.height());
                const QRect r_rect(available_area.x() + l_width, available_area.y(), available_area.width() - l_width, available_area.height());

                calculatePositionsForImages(positions, n->left.get(), l_rect);
                calculatePositionsForImages(positions, n->right.get(), r_rect);
            }
        }
    }

    QRect calculatePositionsForImages(std::vector<QRect>& positions, Node* n, int height)
    {
        const QSize node_size(n->a * height, height);
        const QRect node_rect(QPoint(0, 0), node_size);

        calculatePositionsForImages(positions, n, node_rect);

        return node_rect;
    }

    std::vector<Image> qimagesToImages(const QList<QImage>& qimages)
    {
        std::vector<Image> images;

        for(int i = 0; i < qimages.size(); i++)
            images.emplace_back(qimages[i].width(), qimages[i].height(), i);

        return images;
    }

    std::unique_ptr<Node> generateOptimizedTree(const std::vector<Image>& images)
    {
        auto mutable_images = images;

        auto root = generateTree(mutable_images, images.size(), 1.0);
        assert(mutable_images.empty());      // all images should be used for collage
        recurCalcAR(root.get());

        double prev_a = root->a;
        for(int i = 0; i < 10; i++)
        {
            adjustTree(root.get(), 1.3);
            recurCalcAR(root.get());

            const double current_a = root->a;

            if (std::abs(current_a - prev_a) < 0.01)
                break;
            else
                prev_a = current_a;
        }

        return root;
    }

    QImage generateCollageImage(const QSize& canvas, const QList<QImage>& images_list, const std::vector<QRect>& positions)
    {
        QImage image(canvas, QImage::Format_ARGB32);
        image.fill(Qt::white);

        QPainter painter(&image);

        for(int i = 0; i < images_list.size(); i++)
            painter.drawImage(positions[i], images_list[i]);

        return image;
    }
}


CollageGenerator::CollageGenerator(IExifReader& exifReader)
    : m_exifReader(exifReader)
{

}


QImage CollageGenerator::generateCollage(const QStringList& paths, int height) const
{
    QList<QImage> images;
    std::transform(paths.begin(), paths.end(), std::back_inserter(images), [this](const auto& path)
    {
        return OrientedImage(m_exifReader, path).get();
    });

    const QImage collage = merge(images, height);

    return collage;
}


QImage CollageGenerator::merge(const QList<QImage>& images_list, int height) const
{
    // Images will contain indexes identifying QImages from images_list
    std::vector<Image> images = qimagesToImages(images_list);

    std::sort(images.begin(), images.end(), [](const Image& lhs, const Image& rhs)
    {
        return lhs.a < rhs.a;
    });

    auto root = generateOptimizedTree(images);

    std::vector<QRect> positions;
    const QRect area = calculatePositionsForImages(positions, root.get(), height);
    assert(static_cast<int>(positions.size()) == images_list.size());

    const QImage image = area.isValid()?
        generateCollageImage(area.size(), images_list, positions):
        QImage();

    return image;
}
