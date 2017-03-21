/***************************************************************************
* Copyright (c) 2016, Johan Mabille and Sylvain Corlay                     *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XARRAY_HPP
#define XARRAY_HPP

#include <initializer_list>
#include <utility>
#include <algorithm>

#include "xcontainer.hpp"
#include "xsemantic.hpp"

namespace xt
{

    /********************************
     * xarray_container declaration *
     ********************************/

    template <class EC, class SC>
    struct xcontainer_inner_types<xarray_container<EC, SC>>
    {
        using container_type = EC;
        using shape_type = SC;
        using strides_type = shape_type;
        using backstrides_type = shape_type;
        using inner_shape_type = shape_type;
        using inner_strides_type = strides_type;
        using inner_backstrides_type = backstrides_type;
        using temporary_type = xarray_container<EC, SC>;
    };

    template <class EC, class SC>
    struct xiterable_inner_types<xarray_container<EC, SC>>
        : xcontainer_iterable_types<xarray_container<EC, SC>>
    {
    };

    /**
     * @class xarray_container
     * @brief Dense multidimensional container with tensor semantic.
     *
     * The xarray_container class implements a dense multidimensional container
     * with tensor semantic.
     *
     * @tparam EC The type of the container holding the elements.
     * @tparam SC The type of the containers holding the shape and the strides.
     */
    template <class EC, class SC>
    class xarray_container : public xstrided_container<xarray_container<EC, SC>>,
                             public xcontainer_semantic<xarray_container<EC, SC>>
    {

    public:

        using self_type = xarray_container<EC, SC>;
        using base_type = xstrided_container<self_type>;
        using semantic_base = xcontainer_semantic<self_type>;
        using container_type = typename base_type::container_type;
        using value_type = typename base_type::value_type;
        using reference = typename base_type::reference;
        using const_reference = typename base_type::const_reference;
        using pointer = typename base_type::pointer;
        using const_pointer = typename base_type::const_pointer;
        using shape_type = typename base_type::shape_type;
        using strides_type = typename base_type::strides_type;

        xarray_container();
        explicit xarray_container(const shape_type& shape, layout l = layout::row_major);
        explicit xarray_container(const shape_type& shape, const_reference value, layout l = layout::row_major);
        explicit xarray_container(const shape_type& shape, const strides_type& strides);
        explicit xarray_container(const shape_type& shape, const strides_type& strides, const_reference value);

        xarray_container(const value_type& t);
        xarray_container(nested_initializer_list_t<value_type, 1> t);
        xarray_container(nested_initializer_list_t<value_type, 2> t);
        xarray_container(nested_initializer_list_t<value_type, 3> t);
        xarray_container(nested_initializer_list_t<value_type, 4> t);
        xarray_container(nested_initializer_list_t<value_type, 5> t);

        ~xarray_container() = default;

        xarray_container(const xarray_container&) = default;
        xarray_container& operator=(const xarray_container&) = default;

        xarray_container(xarray_container&&) = default;
        xarray_container& operator=(xarray_container&&) = default;

        template <class E>
        xarray_container(const xexpression<E>& e);

        template <class E>
        xarray_container& operator=(const xexpression<E>& e);

    private:

        container_type m_data;

        container_type& data_impl() noexcept;
        const container_type& data_impl() const noexcept;

        friend class xcontainer<xarray_container<EC, SC>>;
    };

    /******************************
     * xarray_adaptor declaration *
     ******************************/

    template <class EC, class SC = std::vector<typename EC::size_type>>
    class xarray_adaptor;

    template <class EC, class SC>
    struct xcontainer_inner_types<xarray_adaptor<EC, SC>>
    {
        using container_type = EC;
        using shape_type = SC;
        using strides_type = shape_type;
        using backstrides_type = shape_type;
        using inner_shape_type = shape_type;
        using inner_strides_type = strides_type;
        using inner_backstrides_type = backstrides_type;
        using temporary_type = xarray_container<EC, SC>;
    };

    template <class EC, class SC>
    struct xiterable_inner_types<xarray_adaptor<EC, SC>>
        : xcontainer_iterable_types<xarray_adaptor<EC, SC>>
    {
    };

    /**
     * @class xarray_adaptor
     * @brief Dense multidimensional container adaptor with
     * tensor semantic.
     *
     * The xarray_adaptor class implements a dense multidimensional
     * container adaptor with tensor semantic. It is used to provide
     * a multidimensional container semantic and a tensor semantic to
     * stl-like containers.
     *
     * @tparam EC The container type to adapt.
     * @tparam SC The type of the containers holding the shape and the strides.
     */
    template <class EC, class SC>
    class xarray_adaptor : public xstrided_container<xarray_adaptor<EC, SC>>,
                           public xadaptor_semantic<xarray_adaptor<EC, SC>>
    {

    public:

        using self_type = xarray_adaptor<EC, SC>;
        using base_type = xstrided_container<self_type>;
        using semantic_base = xadaptor_semantic<self_type>;
        using container_type = typename base_type::container_type;
        using shape_type = typename base_type::shape_type;
        using strides_type = typename base_type::strides_type;

        xarray_adaptor(container_type& data);
        xarray_adaptor(container_type& data, const shape_type& shape, layout l = layout::row_major);
        xarray_adaptor(container_type& data, const shape_type& shape, const strides_type& strides);

        ~xarray_adaptor() = default;

        xarray_adaptor(const xarray_adaptor&) = default;
        xarray_adaptor& operator=(const xarray_adaptor&);

        xarray_adaptor(xarray_adaptor&&) = default;
        xarray_adaptor& operator=(xarray_adaptor&&);

        template <class E>
        xarray_adaptor& operator=(const xexpression<E>& e);

    private:

        container_type& m_data;

        container_type& data_impl() noexcept;
        const container_type& data_impl() const noexcept;

        using temporary_type = typename xcontainer_inner_types<self_type>::temporary_type;
        void assign_temporary_impl(temporary_type& tmp);

        friend class xcontainer<xarray_adaptor<EC, SC>>;
        friend class xadaptor_semantic<xarray_adaptor<EC, SC>>;
    };

    /***********************************
     * xarray_container implementation *
     ***********************************/

    /**
     * @name Constructors
     */
    //@{
    /**
     * Allocates an uninitialized xarray_container that holds 0 element.
     */
    template <class EC, class SC>
    inline xarray_container<EC, SC>::xarray_container()
        : base_type(), m_data(1, value_type())
    {
    }

    /**
     * Allocates an uninitialized xarray_container with the specified shape and
     * layout.
     * @param shape the shape of the xarray_container
     * @param l the layout of the xarray_container
     */
    template <class EC, class SC>
    inline xarray_container<EC, SC>::xarray_container(const shape_type& shape, layout l)
        : base_type()
    {
        base_type::reshape(shape, l);
    }

    /**
     * Allocates an xarray_container with the specified shape and layout. Elements
     * are initialized to the specified value.
     * @param shape the shape of the xarray_container
     * @param value the value of the elements
     * @param l the layout of the xarray_container
     */
    template <class EC, class SC>
    inline xarray_container<EC, SC>::xarray_container(const shape_type& shape, const_reference value, layout l)
        : base_type()
    {
        base_type::reshape(shape, l);
        std::fill(m_data.begin(), m_data.end(), value);
    }

    /**
     * Allocates an uninitialized xarray_container with the specified shape and strides.
     * @param shape the shape of the xarray_container
     * @param strides the strides of the xarray_container
     */
    template <class EC, class SC>
    inline xarray_container<EC, SC>::xarray_container(const shape_type& shape, const strides_type& strides)
        : base_type()
    {
        base_type::reshape(shape, strides);
    }

    /**
     * Allocates an uninitialized xarray_container with the specified shape and strides.
     * Elements are initialized to the specified value.
     * @param shape the shape of the xarray_container
     * @param strides the strides of the xarray_container
     * @param value the value of the elements
     */
    template <class EC, class SC>
    inline xarray_container<EC, SC>::xarray_container(const shape_type& shape, const strides_type& strides, const_reference value)
        : base_type()
    {
        base_type::reshape(shape, strides);
        std::fill(m_data.begin(), m_data.end(), value);
    }

    /**
     * Allocates an xarray_container that holds a single element initialized to the
     * specified value.
     * @param t the value of the element
     */
    template <class EC, class SC>
    inline xarray_container<EC, SC>::xarray_container(const value_type& t)
        : base_type()
    {
        base_type::reshape(xt::shape<shape_type>(t), layout::row_major);
        nested_copy(m_data.begin(), t);
    }
    //@}

    /**
     * @name Constructors from initializer list
     */
    //@{
    /**
     * Allocates a one-dimensional xarray_container.
     * @param t the elements of the xarray_container
     */
    template <class EC, class SC>
    inline xarray_container<EC, SC>::xarray_container(nested_initializer_list_t<value_type, 1> t)
        : base_type()
    {
        base_type::reshape(xt::shape<shape_type>(t), layout::row_major);
        nested_copy(m_data.begin(), t);
    }

    /**
     * Allocates a two-dimensional xarray_container.
     * @param t the elements of the xarray_container
     */
    template <class EC, class SC>
    inline xarray_container<EC, SC>::xarray_container(nested_initializer_list_t<value_type, 2> t)
        : base_type()
    {
        base_type::reshape(xt::shape<shape_type>(t), layout::row_major);
        nested_copy(m_data.begin(), t);
    }

    /**
     * Allocates a three-dimensional xarray_container.
     * @param t the elements of the xarray_container
     */
    template <class EC, class SC>
    inline xarray_container<EC, SC>::xarray_container(nested_initializer_list_t<value_type, 3> t)
        : base_type()
    {
        base_type::reshape(xt::shape<shape_type>(t), layout::row_major);
        nested_copy(m_data.begin(), t);
    }

    /**
     * Allocates a four-dimensional xarray_container.
     * @param t the elements of the xarray_container
     */
    template <class EC, class SC>
    inline xarray_container<EC, SC>::xarray_container(nested_initializer_list_t<value_type, 4> t)
        : base_type()
    {
        base_type::reshape(xt::shape<shape_type>(t), layout::row_major);
        nested_copy(m_data.begin(), t);
    }

    /**
     * Allocates a five-dimensional xarray_container.
     * @param t the elements of the xarray_container
     */
    template <class EC, class SC>
    inline xarray_container<EC, SC>::xarray_container(nested_initializer_list_t<value_type, 5> t)
        : base_type()
    {
        base_type::reshape(xt::shape<shape_type>(t), layout::row_major);
        nested_copy(m_data.begin(), t);
    }
    //@}


    /**
     * @name Extended copy semantic
     */
    //@{
    /**
     * The extended copy constructor.
     */
    template <class EC, class SC>
    template <class E>
    inline xarray_container<EC, SC>::xarray_container(const xexpression<E>& e)
        : base_type()
    {
        semantic_base::assign(e);
    }

    /**
     * The extended assignment operator.
     */
    template <class EC, class SC>
    template <class E>
    inline auto xarray_container<EC, SC>::operator=(const xexpression<E>& e) -> self_type&
    {
        return semantic_base::operator=(e);
    }
    //@}

    template <class EC, class SC>
    inline auto xarray_container<EC, SC>::data_impl() noexcept -> container_type&
    {
        return m_data;
    }

    template <class EC, class SC>
    inline auto xarray_container<EC, SC>::data_impl() const noexcept -> const container_type&
    {
        return m_data;
    }

    /******************
     * xarray_adaptor *
     ******************/

    /**
     * @name Constructors
     */
    //@{
    /**
     * Constructs an xarray_adaptor of the given stl-like container.
     * @param data the container to adapt
     */
    template <class EC, class SC>
    inline xarray_adaptor<EC, SC>::xarray_adaptor(container_type& data)
        : base_type(), m_data(data)
    {
    }

    /**
     * Constructs an xarray_adaptor of the given stl-like container,
     * with the specified shape and layout.
     * @param data the container to adapt
     * @param shape the shape of the xarray_adaptor
     * @param l the layout of the xarray_adaptor
     */
    template <class EC, class SC>
    inline xarray_adaptor<EC, SC>::xarray_adaptor(container_type& data, const shape_type& shape, layout l)
        : base_type(), m_data(data)
    {
        base_type::reshape(shape, l);
    }

    /**
     * Constructs an xarray_adaptor of the given stl-like container,
     * with the specified shape and strides.
     * @param data the container to adapt
     * @param shape the shape of the xarray_adaptor
     * @param strides the strides of the xarray_adaptor
     */
    template <class EC, class SC>
    inline xarray_adaptor<EC, SC>::xarray_adaptor(container_type& data, const shape_type& shape, const strides_type& strides)
        : base_type(), m_data(data)
    {
        base_type::reshape(shape, strides);
    }
    //@}

    template <class EC, class SC>
    inline auto xarray_adaptor<EC, SC>::operator=(const xarray_adaptor& rhs) -> self_type&
    {
        base_type::operator=(rhs);
        m_data = rhs.m_data;
        return *this;
    }

    template <class EC, class SC>
    inline auto xarray_adaptor<EC, SC>::operator=(xarray_adaptor&& rhs) -> self_type&
    {
        base_type::operator=(std::move(rhs));
        m_data = rhs.m_data;
        return *this;
    }

    /**
     * @name Extended copy semantic
     */
    //@{
    /**
     * The extended assignment operator.
     */
    template <class EC, class SC>
    template <class E>
    inline auto xarray_adaptor<EC, SC>::operator=(const xexpression<E>& e) -> self_type&
    {
        return semantic_base::operator=(e);
    }
    //@}

    template <class EC, class SC>
    inline auto xarray_adaptor<EC, SC>::data_impl() noexcept -> container_type&
    {
        return m_data;
    }

    template <class EC, class SC>
    inline auto xarray_adaptor<EC, SC>::data_impl() const noexcept -> const container_type&
    {
        return m_data;
    }

    template <class EC, class SC>
    inline void xarray_adaptor<EC, SC>::assign_temporary_impl(temporary_type& tmp)
    {
        // TODO (performance improvement) : consider moving tmps
        // shape and strides
        base_type::shape_impl() = tmp.shape();
        base_type::strides_impl() = tmp.strides();
        base_type::backstrides_impl() = tmp.backstrides();
        m_data.resize(tmp.size());
        std::copy(tmp.data().cbegin(), tmp.data().cend(), m_data.begin());
    }
}

#endif
