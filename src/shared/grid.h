#pragma once

class grid
{
public:
    typedef double real;
public:
    inline grid(const coord<real> &mins, const coord<real> &maxs, size_t width, size_t height);

    inline coord<int> getCell(const coord<real> &c) const;
public:
    size_t getWidth()  const {return width_;}
    size_t getHeight() const {return height_;}
private:
    coord<real> mins_;
    coord<real> maxs_;

    coord<real> cell_size_;

    size_t width_;
    size_t height_;
};

grid::grid(const coord<real> &mins, const coord<real> &maxs, size_t width, size_t height)
    : mins_(mins)
    , maxs_(maxs)
    , width_(width)
    , height_(height)
    , cell_size_ ((maxs.x - mins.x) / width, (maxs.y - mins.y) / height)
{

}


coord<int> grid::getCell(const coord<real> &c) const
{
    const coord<real> d = c - mins_;
    return coord<int> (static_cast<int>(d.x / cell_size_.x), static_cast<int>(d.y / cell_size_.y));
}
