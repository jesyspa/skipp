#include "lexer.hpp"
#include <cctype>
#include <iostream>
#define FUSION_MAX_VECTOR_SIZE 30
#include <boost/mpl/vector.hpp>
#include <boost/msm/front/states.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>

namespace {
    namespace msm = boost::msm;
    namespace mpl = boost::mpl;
    using namespace skipp::token;

    struct letter { char c; };
    struct digit { char c; };
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
    struct in_number : msm::front::state<> {
        int val = 0;

        template<typename FSM>
        void on_entry(digit const& sl, FSM&) {
            val *= 10;
            val += sl.c-'0';
        }

        template<typename EVENT, typename FSM>
        void on_exit(EVENT const&, FSM& fsm) {
            fsm.emit(number{val});
            val = 0;
        }

        template<typename FSM>
        void on_exit(digit const&, FSM&) {}
    };

    struct lexer_ : msm::front::state_machine_def<lexer_> {
        tokens result;

        using initial_state = empty;

        template<typename E, typename FSM>
        void on_exit(E const&, FSM&) {
            emit(eof{});
        }

        void emit(token const& tok) {
            result.push_back(tok);
        }

        using m = lexer_;
        struct transition_table : mpl::vector<
            _row <empty,       letter,      in_variable>,
            _row <empty,       digit,       in_number>,
            _row <empty,       whitespace,  empty>,
            a_row<empty,       token,      empty,        &m::emit>,
            _row <in_variable, letter,      in_variable>,
            _row <in_variable, digit,       in_number>,
            _row <in_variable, whitespace,  empty>,
            a_row<in_variable, token,      empty,        &m::emit>,
            _row <in_number,   digit,       in_number>,
            _row <in_number,   letter,      in_variable>,
            _row <in_number,   whitespace,  empty>,
            a_row<in_number,   token,       empty,       &m::emit>
        > {};
    };

    using lexer = msm::back::state_machine<lexer_>;
}

namespace skipp {

token::tokens lex(std::string const& s) {
    lexer lexer;
    using token::token;
    lexer.start();
    for (char c : s) {
        if (c == '.')
            lexer.process_event(token(dot{}));
        else if (c == '\\')
            lexer.process_event(token(lambda{}));
        else if (c == '(')
            lexer.process_event(token(open_paren{}));
        else if (c == ')')
            lexer.process_event(token(close_paren{}));
        else if (std::isspace(c))
            lexer.process_event(whitespace{});
        else if (std::isdigit(c))
            lexer.process_event(digit{c});
        else
            lexer.process_event(letter{c});

    }
    lexer.stop();
    return lexer.result;
}

} // skipp
