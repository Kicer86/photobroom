
#ifndef IACTION_HPP
#define IACTION_HPP

namespace Database
{
    class IAction
    {
        public:
            ~IAction() = default;

            virtual QString decoratePhotoQuery(const QString &) = 0;
    };
}

#endif
