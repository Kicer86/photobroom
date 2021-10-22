
#ifndef OPENLIBRARY_PALGORITHM_TS_RESOURCE
#define OPENLIBRARY_PALGORITHM_TS_RESOURCE

#include <cassert>

#include <mutex>
#include <memory>
#include <ostream>

namespace ol
{

    //! Thread safe wrapper for objects.
    /*!
    ThreadSafeResource is a template class which allows to wrap any object (resource) into a thread safe container.
    Resource object must be constructed together with ThreadSafeResource (see ThreadSafeResource::ThreadSafeResource()).
    Then it can accessed by one thread at once. Any access to resource when it is already locked will cause in wait operation.
    To access locked resource call ThreadSafeResource::lock(). An Accessor object will be returned which is responsible for resource unlocking 
    when no longer used.
    */

    template<typename T>
    class ThreadSafeResource
    {
        public:
            //! Notification interface
            struct INotify
            {
                virtual ~INotify() {}
                virtual void unlocked() = 0;                       //!< Called when Accessor is being destructed and resource is unlocked.
            };

            //! ThreadSafeResource's helper class
            /*! Accessor is used by ThreadSafeResource for giving exclusive access to resource.
            *  Resource is normally being kept in ThreadSafeResource object.
            *  When ThreadSafeResource::lock() is called an Accessor object is returned with exclusive access to resource.
            *  As long as Accessor is alive, resource cannot be accessed by other threads.
            */
            struct Accessor
            {
                //! Constructor
                //! Constructs an Accessor which is temporary owner of locked resource. Until Accessor is destructed resource is locked and can be accessed safely by Accessor's client
                Accessor(std::mutex& mutex, T* object): m_lock(mutex), m_object(object), m_notify(nullptr) {}

                //! Move Constructor
                Accessor(Accessor&& other): m_lock(std::move(other.m_lock)),
                                            m_object(other.m_object),
                                            m_notify(other.m_notify)
                {
                    other.m_object = nullptr;
                    other.m_notify = nullptr;
                }

                //! Constructor
                //! Constructs an invalid Accessor. is_valid() will return false.
                Accessor(): m_lock(), m_object(nullptr), m_notify(nullptr) {}

                //! Constructor
                Accessor(std::unique_lock<std::mutex>&& lock, T* object): m_lock(std::move(lock)), m_object(object), m_notify(nullptr) {}
                Accessor(const Accessor &) = delete;

                Accessor& operator=(const Accessor &) = delete;

                //! Move operator
                Accessor& operator=(Accessor&& other)
                {
                    m_lock = std::move(other.m_lock);
                    m_object = other.m_object;
                    m_notify = other.m_notify;

                    other.m_object = nullptr;
                    other.m_notify = nullptr;

                    return *this;
                }

                //! Destructor
                /*!
                * After destruction of Accessor resource is again available to other threads.
                * If INotify object was used with ThreadSafeResource::lock(INotify *), unlock() will be called on it
                */
                virtual ~Accessor()
                {
                    if (m_lock)
                        m_lock.unlock();

                    if (m_notify != nullptr)
                        m_notify->unlocked();
                }

                //! get reference to resource
                T& get()
                {
                    return *m_object;
                }

                //! get reference to const resource
                const T& get() const
                {
                    return *m_object;
                }

                //! get pointer to resource
                T* operator->()
                {
                    return m_object;
                }

                //! get pointer to const resource
                const T* operator->() const
                {
                    return m_object;
                }

                //! get reference to resource
                T& operator*()
                {
                    return *m_object;
                }

                //! get reference to const resource
                const T& operator*() const
                {
                    return *m_object;
                }

                //! Check if Accessor is valid.
                /*! Returns true when Accessor holds resource. Otherwise returns false.
                    Only default constructor Accessor() makes it invalid. All other constructors will create a valid Accessor object. */
                bool is_valid() const
                {
                    return m_object != nullptr;
                }

                private:
                    friend class ThreadSafeResource;
                    std::unique_lock<std::mutex> m_lock;
                    T* m_object;
                    INotify* m_notify;

                    void set(INotify* notify)
                    {
                        m_notify = notify;
                    }
            };

            //! Contructor
            /*! Constructs ThreadSafeResource together with locked resource. */
            template<typename... Args>
            ThreadSafeResource(const Args&... args): m_mutex(), m_resource(args...)
            {
            }

            //! Destructor
            //! Destructs resource object. If it is still locked by some Accessor, destructor will wait until resource is released.
            ~ThreadSafeResource()
            {
                // point of this lock is te be sure resource is not used (no live Accessors exist)
                std::unique_lock<std::mutex> l(m_mutex);
            }

            ThreadSafeResource(const ThreadSafeResource<T> &) = delete;
            ThreadSafeResource<T>& operator=(const ThreadSafeResource<T> &) = delete;

            //! Locks a resource and returns access to it.
            //! As long as returned Accessor object exists the resource will be locked and unavailable for other threads.
            Accessor lock()
            {
                Accessor accessor(m_mutex, &m_resource);

                return accessor;
            }

            //! Locks a resource and returns access to it.
            //! Behaves as lock() but calls INotify::unlocked() when object is being unlocked.
            Accessor lock(INotify* notify)
            {
                Accessor accessor = lock();
                accessor.set(notify);

                return accessor;
            }


            //! Tries to lock resource.
            //! Invalid Accessor will be returned when lock failed.
            Accessor try_lock()
            {
                std::unique_lock<std::mutex> l(m_mutex, std::defer_lock);
                const bool locked = l.try_lock();
                Accessor result;

                if (locked)
                    result = Accessor(std::move(l), &m_resource);

                return result;
            }

        private:
            std::mutex m_mutex;
            T m_resource;
    };

}

#endif
