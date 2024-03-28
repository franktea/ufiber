//
// Copyright (c) 2018 Damian Jarek (damian dot jarek93 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/djarek/ufiber
//

#ifndef UFIBER_IMPL_UFIBER_HPP
#define UFIBER_IMPL_UFIBER_HPP

#include <ufiber/ufiber.hpp>
#include <concepts>

namespace ufiber
{

template<class Executor>
yield_token<Executor>::yield_token(Executor const& ex,
                                   detail::fiber_context& ctx)
  : ctx_{ctx}
  , executor_{ex}
{
}
template<class Executor>
typename yield_token<Executor>::executor_type
yield_token<Executor>::get_executor() noexcept
{
    return executor_;
}

template<class E, class F>
    requires boost::asio::is_executor<E>::value
auto spawn(E const& ex, F&& f)
{
    detail::initial_resume(
      boost::context::fiber{detail::fiber_main<typename std::decay<F>::type, E>{
        std::forward<F>(f), ex}});
}

template<class Ctx, class F>
    requires std::convertible_to<Ctx&, boost::asio::execution_context&>
auto spawn(Ctx& ctx, F&& f)
{
    detail::initial_resume(
      boost::context::fiber{detail::fiber_main<typename std::decay<F>::type,
                                               typename Ctx::executor_type>{
        std::forward<F>(f), ctx.get_executor()}});
}

template<class Alloc, class Executor, class F>
void
spawn(std::allocator_arg_t arg, Alloc&& sa, Executor const& ex, F&& f)
{
    detail::initial_resume(boost::context::fiber{
      arg,
      std::forward<Alloc>(sa),
      detail::fiber_main<typename std::decay<F>::type, Executor>{
        std::forward<F>(f), ex}});
}
} // namespace ufiber

#endif // UFIBER_IMPL_UFIBER_HPP
