
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
        int idx = -1;
        double a = 1.0;

        bool operator<(const Image& other) const
        {
            return a < other.a;
        }
    };

    struct Node
    {
        bool isLeaf = false;
        double a_tar = 1.0;
        double a = 1.0;
        bool horizontalSplit = false;

        Image image;

        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };

    std::pair<Image, Image> takeImgPair(std::multiset<Image>& images, double a)
    {
        auto p = images.begin();
        auto q = prev(images.end());
        auto i = p;
        auto j = q;
        double m = std::abs(p->a + q->a - a);

        while (p != q)
        {
            const double a_sum = p->a + q->a;

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
            else if (a_sum <= a)
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

        Image l = *i;
        Image r = *j;

        images.erase(i);
        images.erase(j);

        return std::make_pair(l, r);
    }


    Image takeImage(std::multiset<Image>& images, double a)
    {
        auto best = std::min_element(images.begin(), images.end(), [a](const Image& lhs, const Image& rhs)
        {
            return std::abs(lhs.a - a) < std::abs(rhs.a - a);
        });

        Image image = *best;
        images.erase(best);

        return image;
    }

    std::unique_ptr<Node> generateTree(std::multiset<Image>& images, int leafs, double ratio)
    {
        std::unique_ptr<Node> node = std::make_unique<Node>();
        node->a_tar = ratio;
        node->isLeaf = leafs == 1;

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
                left->isLeaf = true;
                right->image = imagePair.second;
                right->isLeaf = true;

                node->left = std::move(left);
                node->right = std::move(right);
            }
        }

        return node;
    }

    double recurCalcAR(Node* n)
    {
        if (n->isLeaf)
            n->a = n->a_tar = n->image.a;
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
        if (n->isLeaf == false)
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
        if (n->isLeaf)
        {
            const int idx = n->image.idx;
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
}


CollageGenerator::CollageGenerator(IExifReader& exifReader)
    : m_exifReader(exifReader)
{

}


QImage CollageGenerator::generateCollage(const QStringList& paths) const
{
    QStringList toUse;

    const auto count = paths.size();

    if (count <= 40)
        toUse = paths;
    else
    {
        const int step = std::round(count / 4.0);

        toUse.push_back(paths[0]);
        toUse.push_back(paths[step]);
        toUse.push_back(paths[count - 1 - step]);
        toUse.push_back(paths[count - 1]);
    }

    QList<QImage> images;
    std::transform(toUse.begin(), toUse.end(), std::back_inserter(images), [this](const auto& path)
    {
        return OrientedImage(m_exifReader, path).get();
    });

    const QImage collage = merge(images);

    return collage;
}


QImage CollageGenerator::merge(const QList<QImage>& images_list) const
{
    std::multiset<Image> images;

    for(int i = 0; i < images_list.size(); i++)
    {
        const QImage& qimage = images_list[i];
        const double ratio = static_cast<double>(qimage.width()) / qimage.height();
        Image image;
        image.a = ratio;
        image.idx = i;

        images.insert(image);
    }

    auto root = generateTree(images, images_list.size(), 1.0);
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

    std::vector<QRect> positions;
    const QRect area = calculatePositionsForImages(positions, root.get(), 1024);

    QImage image(area.size(), QImage::Format_ARGB32);
    image.fill(Qt::white);

    QPainter painter(&image);

    for(int i = 0; i < images_list.size(); i++)
        painter.drawImage(positions[i], images_list[i]);

    return image;
}
