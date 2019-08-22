
#ifndef ITHUMBNAIL_MANAGER_HPP
#define ITHUMBNAIL_MANAGER_HPP

#include <memory>

#include <QImage>
#include <QString>


struct AThumbnailGenerator
{
    virtual ~AThumbnailGenerator() = default;

    template<typename C>
    void generate(const QString& path, int desired_height, C&& c)
    {
        struct Callback: ICallback
        {
            explicit Callback(C&& c): m_c(std::move(c)) {}

            void result(const QImage& result) override
            {
                m_c(result);
            }

            C m_c;
        };

        run(path, desired_height, std::make_unique<Callback>(std::move(c)));
    }

    protected:
        struct ICallback
        {
            virtual ~ICallback() = default;

            virtual void result(const QImage &) = 0;
        };

        virtual void run(const QString &, int, std::unique_ptr<ICallback>) = 0;
};


struct AThumbnailManager
{
    AThumbnailManager(AThumbnailGenerator* gen): m_generator(gen) {}
    virtual ~AThumbnailManager() = default;

    template<typename C>
    void fetch(const QString& path, int desired_height, C&& c)
    {
        const QImage cached = find(path, desired_height);

        if (cached.isNull())
            m_generator->generate(path, desired_height, [this, callback{std::move(c)}, desired_height, path] (const QImage& img) mutable
            {
                const int height = img.height();
                assert(height == desired_height || img.isNull());

                cache(path, height, img);
                callback(height, img);
            });
        else
            c(desired_height, cached);
    }

    private:
        AThumbnailGenerator* m_generator;

        virtual QImage find(const QString &, int) = 0;
        virtual void cache(const QString &, int, const QImage &) = 0;
};

#endif
