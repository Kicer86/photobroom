
#ifndef OPENLIBRARY_UTILS_COPY_PTR
#define OPENLIBRARY_UTILS_COPY_PTR

namespace ol
{

    //deleter
    namespace OpenLibrary_CopyPtr
    {
        template<class T>
        void deleter(T *ptr)
        {
            delete ptr;
        }

        template<class T>
        void copier(T* &to_ptr, const T *from_ptr)
        {
            if (from_ptr == nullptr)
                to_ptr = new T;
            else
                to_ptr = new T(*from_ptr);
        }
    }


    //! Copyable data smart pointer.

    //! When data_ptr is copied, it makes a copy of hold object. So it behaves similarly std::shared_ptr,
    //! but instead of pointing to the same data a copy of data occurs.
    //! data_ptr can be used for storing class' private data which on class copy should also be copied.
    //! operator= of data_ptr and copy constructor of data_ptr are calling copy constructor of encapsulated type.
    template<class T>
    class data_ptr
    {
        public:
            typedef void (*Deleter)(T *);                             //!< deleter for hold object
            typedef void (*Copier)(T* &, const T *);                  //!< interface of copying function

            //! Constructor.
            /** Constructs data_ptr object and stores pointer to object of type T inside. data_ptr takes ownership over passed pointer.
            * @arg d object deleter. A function which takes pointer to T as parameter. Responsibility of deleter is to delete owned pointer when data_ptr is destroyed.
            * @arg c object copier. A function of type data_ptr::Copier. It is called on data_ptr copy. Its responsibility is to create new instance of T and initialize it with data from copied data_ptr.
            */
            data_ptr(T *ptr = nullptr, Deleter d = &OpenLibrary_CopyPtr::deleter, Copier c = &OpenLibrary_CopyPtr::copier): m_ptr(ptr), m_deleter(d), m_copier(c)
            {

            }

            //! Copy constructor
            /** Makes copy of data_ptr.
            *  Object kept in data_ptr will be copied too.
            */
            data_ptr(const data_ptr<T> &other): m_ptr(0), m_deleter(nullptr), m_copier(nullptr)
            {
                copy(other);
            }

            //! Move constructor.
            /** Moves data from one data_ptr to another.
            *  No copies are made. Source data_ptr becomes empty.
            */
            data_ptr(data_ptr<T> &&other): m_ptr(0), m_deleter(nullptr), m_copier(nullptr)
            {
                move(other);
                other.m_ptr = nullptr;
            }

            virtual ~data_ptr()
            {
                del();
            }

            //takes ownership over data
            data_ptr& operator=(T *ptr)
            {
                if (m_ptr != ptr)
                {
                    del();
                    m_ptr = ptr;
                }

                return *this;
            }

            data_ptr& operator=(const data_ptr<T> &other)
            {
                if ( &other != this )
                    copy(other);

                return *this;
            }

            T* operator->()
            {
                return m_ptr;
            }

            const T* operator->() const
            {
                return m_ptr;
            }

            T& operator *()
            {
                return *m_ptr;
            }

            const T& operator *() const
            {
                return *m_ptr;
            }

            T* get()
            {
                return m_ptr;
            }

            const T* get() const
            {
                return m_ptr;
            }

        private:
            T *m_ptr;
            Deleter m_deleter;
            Copier m_copier;

            //getting operators
            Deleter getDeleter() const
            {
                return m_deleter;
            }

            Copier getCopier() const
            {
                return m_copier;
            }

            //copy another copy_ptr
            void copy(const data_ptr<T> &other)
            {
                //copy operators
                m_deleter = other.m_deleter;
                m_copier = other.m_copier;

                //copy data
                copy(other.m_ptr);
            }


            //move another copy_ptr
            void move(const data_ptr<T> &other)
            {
                m_deleter = other.m_deleter;
                m_copier = other.m_copier;
                m_ptr = other.m_ptr;
            }

            //copy object
            void copy(T *ptr)
            {
                //delete old data
                del();

                //create copy
                getCopier()(m_ptr, ptr);
            }

            //delete object
            void del() const
            {
                getDeleter()(m_ptr);
            }

    };

}

#endif
