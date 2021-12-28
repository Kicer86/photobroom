
#ifndef ICOMPLETER_FACTORY_HPP
#define ICOMPLETER_FACTORY_HPP

class QCompleter;
class QAbstractItemModel;

class TagTypeInfo;
class IModelCompositorDataSource;


struct ICompleterFactory
{
    virtual ~ICompleterFactory() {}

    /**
     * @brief create QCompleter for given type
     * \return Pointer to QCompleter. It is client's responsibility to delete it
     */
    virtual QCompleter* createCompleter(const Tag::Types &) = 0;

    /**
     * @brief create QCompleter for given set of types
     * \return Pointer to QCompleter. It is client's responsibility to delete it
     */
    virtual QCompleter* createCompleter(const std::set<Tag::Types> &) = 0;

    /**
     * @brief create QCompleter with people names
     * \return Pointer to QCompleter. It is client's responsibility to delete it
     */
    virtual QCompleter* createPeopleCompleter() = 0;

    /**
     * @brief access model with values for given tag type
     * \return Pointer to IModelCompositorDataSource - completition list.
     */
    virtual IModelCompositorDataSource* accessModel(const Tag::Types &) = 0;

    /**
     * @brief access model with list of people names
     * \return Pointer to IModelCompositorDataSource
     */
    virtual const IModelCompositorDataSource& accessPeopleModel() = 0;
};

#endif
