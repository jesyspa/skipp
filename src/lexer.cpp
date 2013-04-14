#include "lexer.hpp"
#include <cctype>
#include <iostream>
#include <boost/msm/front/states.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/mpl/vector.hpp>

namespace {
    namespace msm = boost::msm;
    namespace mpl = boost::mpl;
    using namespace skipp::token;

    struct letter { char c; };
    struct whitespace {};

    struct empty : msm::front::state<> {};
    struct in_variable : msm::front::state<> {
        std::string name;

        template<typename FSM>
        void on_entry(letter const& sl, FSM&) {
            name += sl.c;
        }

        template<typename EVENT, typename FSM>
        void on_exit(EVENT const&, FSM& fsm) {
            fsm.emit(variable{name});
            name.clear();
        }

        template<typename FSM>
        void on_exit(letter const&, FSM&) {}
    };

    struct lexer_ : msm::front::state_machine_def<lexer_> {
        tokens result;

        using initial_state = empty;

        template<typename E, typename FSM>
        void on_exit(E const&, FSM&) {
            emit(eof{});
        }

        template<typename T>
        void emit(T const& tok) {
            result.push_back(tok);
        }

        using m = lexer_;
        struct transition_table : mpl::vector<
            a_row<empty,       dot,         empty,        &m::emit>,
            a_row<empty,       lambda,      empty,        &m::emit>,
            a_row<empty,       open_paren,  empty,        &m::emit>,
            a_row<empty,       close_paren, empty,        &m::emit>,
            _row <empty,       letter,      in_variable>,
            _row <empty,       whitespace,  empty>,
            _row <in_variable, letter,      in_variable>,
            a_row<in_variable, dot,         empty,        &m::emit>,
            a_row<in_variable, lambda,      empty,        &m::emit>,
            a_row<in_variable, open_paren,  empty,        &m::emit>,
            a_row<in_variable, close_paren, empty,        &m::emit>,
            _row <in_variable, whitespace,  empty>
        > {};
    };

    using lexer = msm::back::state_machine<lexer_>;
}

namespace skipp {
namespace lexer {

token::tokens lex(std::string const& s) {
    ::lexer lexer;
    lexer.start();
    for (char c : s) {
        if (c == '.')
            lexer.process_event(dot{});
        else if (c == '\\')
            lexer.process_event(lambda{});
        else if (c == '(')
            lexer.process_event(open_paren{});
        else if (c == ')')
            lexer.process_event(close_paren{});
        else if (std::isspace(c))
            lexer.process_event(whitespace{});
        else
            lexer.process_event(letter{c});

    }
    lexer.stop();
    return lexer.result;
}

} // lexer
} // skipp
