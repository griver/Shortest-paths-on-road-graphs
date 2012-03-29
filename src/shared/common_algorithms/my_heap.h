#pragma once

template <typename T>
class my_heap
{
public:
    typedef boost::function<bool(const T&, const T&)> less_fn;
    typedef boost::function<void(const T&, size_t)> callback_fn;
    //typedef boost::function<void(std::pair<const T*, size_t>)> callback_fn;

    inline my_heap (const callback_fn& clb)
    : clb_(clb)
    , changed_top (0)
    , changed_bottom (0)
    {

    }

    void my_heap::reserve (size_t size)
    {
        arr_.reserve(size);
    }
 
    inline size_t insert( const T& v )
    {
        arr_.push_back (v);
        return decrease_key (arr_.size() - 1, v);
    }
    
    inline void pop( bool skip_callback = false )
    {
        arr_[0] = arr_[arr_.size()-1];
        arr_.pop_back();
        if (!empty())
            heapify(skip_callback);
    }

    inline void callback (const T& t, size_t i)
    {
        if (clb_ == NULL)
            return;
        //std::pair<const T*, size_t> p (&t, i);
        clb_(t, i);
    }

    size_t decrease_key ( size_t i, const T& new_val )
    {

        size_t j;
        arr_[i] = new_val;
        while (i > 0 && less_(arr_[i], arr_[j = parent(i)]))
        {
            std::swap (arr_[i], arr_[j]);
            callback (arr_[i], i);
            i = j;
        }
        callback (arr_[i], i);
        return i;
    }

    inline void clear( )
    {
        arr_.clear ();
    }

    inline const T& get( size_t index ) const
    {
        return arr_[index];
    }

    inline size_t size( ) const
    {
        return arr_.size ();
    }

    inline bool empty( ) const
    {
        return arr_.empty();
    }
    

    inline static size_t parent( size_t i ) 
    {
        return (i+1)/2 - 1;
    }
private:
    
    inline size_t get_least ( size_t i )
    {
        size_t l = (i+1) * 2 - 1;
        size_t r = (i+1) * 2;
        size_t least = i;

        if (l<arr_.size() && less_(arr_[l], arr_[i]))
            least = l;

        if (r<arr_.size() && less_(arr_[r], arr_[least]))
            least = r;

        return least;
    }

    void heapify ( bool skip_callback = false )
    {
        size_t i = 0;
        changed_top = i;

        size_t least;
        while ((least=get_least(i)) != i)
        {
            std::swap(arr_[i], arr_[least]);
            if (!skip_callback)
                callback (arr_[i], i);
            i = least;
        }


        if (!skip_callback)
            callback (arr_[i], i);
        
        changed_bottom = i;
    }

    bool less_ (const T& a, const T& b)
    {
        return (a < b);
    }

    std::vector<T> arr_;
    callback_fn clb_;
    //comparator

public:
    size_t changed_top, changed_bottom;
public:
    DWORD time;


};