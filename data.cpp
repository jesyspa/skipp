#include <cassert>
#include <cstdio>
#include <memory>

#ifdef __GNUC__
#define WARN_UNUSED __attribute__((warn_unused_result))
#endif

enum node_type {
    empty,
    application_node,
    number_node,
    function_node
};

struct stack_link;
using stack = std::unique_ptr<stack_link>;


struct object {
    int ref_count;
};

struct ref {
    node_type type;
    object* node;
};

struct application_impl {
    ref left, right;
    static constexpr node_type type = application_node;
};

struct number_impl {
    int value;
    static constexpr node_type type = number_node;
};

using func_t = ref (*)(stack&);

struct function_impl {
    func_t func;
    static constexpr node_type type = function_node;
};

template<typename T>
struct objectify : object, T {};

using application = objectify<application_impl>;
using number = objectify<number_impl>;
using function = objectify<function_impl>;

struct stack_link {
    stack prev;
    application* arg;
};


WARN_UNUSED
application* make_application(ref left, ref right) {
    auto app = new application;
    app->ref_count = 1;
    app->left = left;
    app->right = right;
    return app;
}

WARN_UNUSED
number* make_number(int value) {
    auto num = new number;
    num->ref_count = 1;
    num->value = value;
    return num;
}

WARN_UNUSED
function* make_function(func_t func) {
    auto fun = new function;
    fun->ref_count = 1;
    fun->func = func;
    return fun;
}

template<typename T>
ref to_ref(T* obj) {
    return {T::type, obj};
}

template<typename T>
bool is(ref r) {
    return r.type == T::type;
}

template<typename T>
T* from_ref(ref r) {
    assert(is<T>(r) && "type mismatch");
    return reinterpret_cast<T*>(r.node);
}

template<typename T>
T* maybe_from_ref(ref r) {
    if (is<T>(r))
        return reinterpret_cast<T*>(r.node);
    return nullptr;
}

ref eval(ref);

template<typename T>
T* eval_as(ref r) {
    return from_ref<T>(eval(r));
}

void rm_object(object* obj) {
    assert(obj && "deleting non-existant object");
    if (!--obj->ref_count)
        delete obj;
}

void share_object(object* obj) {
    assert(obj && "sharing non-existant object");
    ++obj->ref_count;
}

void push(stack& sl, application* app) {
    share_object(app);
    stack p(new stack_link{nullptr, app});
    p->prev.swap(sl);
    p.swap(sl);
}

WARN_UNUSED
application* extract(stack& sl) {
    assert(sl && "extracting from empty stack");
    auto app = sl->arg;
    stack local;
    local.swap(sl);
    sl.swap(local->prev);
    return app;
}

void pop(stack& sl) {
    assert(sl && "popping empty stack");
    rm_object(sl->arg);
    stack local;
    local.swap(sl);
    sl.swap(local->prev);
}

ref add(stack& sl) {
    auto lhs = extract(sl);
    auto rhs = extract(sl);
    auto result = make_number(
        eval_as<number>(lhs->right)->value
        + eval_as<number>(rhs->right)->value);
    rm_object(lhs);
    rm_object(rhs);
    return to_ref(result);
}

ref comb_i(stack& sl) {
    auto arg = extract(sl);
    auto result = arg->right;
    rm_object(arg);
    return result;
}

ref comb_k(stack& sl) {
    auto arg = extract(sl);
    rm_object(extract(sl));
    auto result = arg->right;
    rm_object(arg);
    return result;
}

ref comb_s(stack& sl) {
    auto f = extract(sl);
    auto g = extract(sl);
    auto x = extract(sl);
    share_object(x->right.node);
    auto result = to_ref(make_application(
        to_ref(make_application(f->right, x->right)),
        to_ref(make_application(g->right, x->right))));
    rm_object(f);
    rm_object(g);
    rm_object(x);
    return result;
}

ref comb_l(stack& sl) {
    auto f = extract(sl);
    auto g = extract(sl);
    auto x = extract(sl);
    auto result = to_ref(make_application(
        to_ref(make_application(f->right, x->right)),
        g->right));
    rm_object(f);
    rm_object(g);
    rm_object(x);
    return result;
}

ref comb_r(stack& sl) {
    auto f = extract(sl);
    auto g = extract(sl);
    auto x = extract(sl);
    auto result = to_ref(make_application(
        f->right,
        to_ref(make_application(g->right, x->right))));
    rm_object(f);
    rm_object(g);
    rm_object(x);
    return result;
}

ref print(stack& sl) {
    auto arg = extract(sl);
    auto val = eval(arg->right);
    std::printf("%d\n", from_ref<number>(val)->value);
    rm_object(arg);
    return val;
}

ref update(application* app, ref newval) {
    if (auto* napp = maybe_from_ref<application>(newval)) {
        app->left = napp->left;
        app->right = napp->right;
    } else {
        app->left = to_ref(make_function(comb_i));
        app->right = newval;
    }
    return to_ref(app);
}

void print(ref r) {
    if (auto* app = maybe_from_ref<application>(r)) {
        printf("(");
        print(app->left);
        printf(" ");
        print(app->right);
        printf(")");
    } else if (auto* n = maybe_from_ref<number>(r)) {
        printf("%d", n->value);
    } else {
        printf("?");
    }
}

ref eval(ref r) {
    stack s;
    auto* orig = maybe_from_ref<application>(r);
    while (true) {
        while (auto* app = maybe_from_ref<application>(r)) {
            push(s, app);
            r = app->left;
        }

        assert(is<function>(r) && "type error: trying to apply non-func");
        auto* f = from_ref<function>(r);
        bool id = f->func == comb_i;

        auto result = f->func(s);
        if (!s) {
            if (is<application>(result))
                r = update(orig, result);
            else
                return result;
        } else if (id)
            r = s->arg->left = result;
        else
            r = update(s->arg, result);
    }
}

ref app_ref(ref left, ref right) {
    return to_ref(make_application(left, right));
}

ref num_ref(int val) {
    return to_ref(make_number(val));
}

ref fun_ref(func_t f) {
    return to_ref(make_function(f));
}

int main() {
    auto expr = app_ref(
        fun_ref(print),
        app_ref(
            app_ref(
                app_ref(
                    fun_ref(comb_s),
                    app_ref(
                        app_ref(
                            fun_ref(comb_r),
                            fun_ref(add)),
                        fun_ref(comb_i))),
                fun_ref(comb_i)),
            num_ref(3)));
    eval(expr);
}
