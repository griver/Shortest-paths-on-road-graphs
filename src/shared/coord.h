#pragma once

template <typename T>
struct coord : boost::addable<coord<T> >, boost::subtractable<coord<T> >, boost::multipliable2<coord<T>, T>
{
    typedef T value_type;

    coord () : x(0), y(0) {}
    coord (const T& X, const T& Y) : x(X), y(Y) {}

    coord& operator+= (const coord<T>& o)
    {
        x+=o.x;
        y+=o.y;
        return *this;
    }
    coord& operator-= (const coord<T>& o)
    {
        x-=o.x;
        y-=o.y;
        return *this;
    }
    coord& operator*= (const T& m)
    {
        x*=m;
        y*=m;
        return *this;
    }

    template<typename D>
    operator coord<D> () const
    {
        coord<D> res (static_cast<D>(x), static_cast<D>(y));
        return res;
    }

    T x, y;
};

template<typename T>
std::ostream& operator<< (std::ostream& s, const coord<T>& c)
{
    s << c.x << ", " << c.y;
    return s;
}
