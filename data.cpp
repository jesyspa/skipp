#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <list>

#ifdef __GNUC__
#define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#endif

enum object_type {
    application_object,
    number_object,
    function_object
};

struct stack_link;
using stack = std::unique_ptr<stack_link>;

struct object {
    bool used;
    object_type type;
    object* next;
};

using ref = object*;

std::list<ref> root_set;
ref first;
unsigned int obj_count;

class root {
    using iter_t = decltype(root_set)::iterator;
    iter_t it;

public:
    root(ref p) {
        root_set.push_front(p);
        it = root_set.begin();
    }
    root(root const&) = delete;
    root(root&& o) : it(o.it) {
        o.it = iter_t{};
    }
    root& operator=(root const&) = delete;
    root& operator=(root&& o) {
        swap(o);
        return *this;
    }
    ~root() {
        root_set.erase(it);
    }
    void swap(root& o) {
        std::swap(it, o.it);
    }
};

template<typename T>
class safe_ref {
    root protector;
    T* ptr;

public:
    safe_ref(T* p) : protector(p), ptr(p) {}

    operator T*() const {
        return ptr;
    }
};

struct application : object {
    ref left, right;
    static constexpr object_type TYPE = application_object;
};

struct number : object {
    int value;
    static constexpr object_type TYPE = number_object;
};

using func_t = ref (*)(stack&);

struct function : object {
    func_t func;
    static constexpr object_type TYPE = function_object;
};

struct stack_link {
    stack prev;
    application* arg;
};

void collect_garbage();

template<typename T>
WARN_UNUSED_RESULT
T* new_object() {
    auto obj = static_cast<T*>(malloc(sizeof(T)));
    obj->next = nullptr;
    obj->type = T::TYPE;
    // For debugging purposes, we always collect garbage.
    collect_garbage();
    if (!first) {
        first = obj;
    } else {
        obj->next = first;
        first = obj;
    }
    return obj;
}


WARN_UNUSED_RESULT
safe_ref<application> make_application(ref left, ref right) {
    auto app = new_object<application>();
    app->left = left;
    app->right = right;
    return app;
}

WARN_UNUSED_RESULT
safe_ref<number> make_number(int value) {
    auto num = new_object<number>();
    num->value = value;
    return num;
}

WARN_UNUSED_RESULT
safe_ref<function> make_function(func_t func) {
    auto fun = new_object<function>();
    fun->func = func;
    return fun;
}

template<typename T>
bool is(ref r) {
    return r->type == T::TYPE;
}

template<typename T>
T* cast(ref r) {
    assert(is<T>(r) && "type mismatch");
    return reinterpret_cast<T*>(r);
}

template<typename T>
T* try_cast(ref r) {
    if (is<T>(r))
        return reinterpret_cast<T*>(r);
    return nullptr;
}

void walk(ref r) {
    if (r->used)
        return;
    r->used = true;
    if (auto* app = try_cast<application>(r)) {
        walk(app->left);
        walk(app->right);
    }
}

void collect_garbage() {
    for (auto p = first; p; p = p->next)
        p->used = false;

    for (auto p : root_set)
        walk(p);

    while (first && !first->used) {
        auto* p = first->next;
        free(first);
        first = p;
    }
    if (!first)
        return;
    for (auto p = first; p->next; p = p->next) {
        if (p->next->used)
            continue;
        auto* next = p->next;
        p->next = next->next;
        free(next);
    }
}
ref eval(ref);

template<typename T>
T* eval_as(ref r) {
    return cast<T>(eval(r));
}

void push(stack& sl, application* app) {
    stack p(new stack_link{nullptr, app});
    p->prev.swap(sl);
    p.swap(sl);
}

WARN_UNUSED_RESULT
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
    return result;
}

ref comb_i(stack& sl) {
    auto arg = extract(sl);
    auto result = arg->right;
    return result;
}

ref comb_k(stack& sl) {
    auto arg = extract(sl);
    auto result = arg->right;
    return result;
}

ref comb_s(stack& sl) {
    auto f = extract(sl);
    auto g = extract(sl);
    auto x = extract(sl);
    auto result = make_application(
        make_application(f->right, x->right),
        make_application(g->right, x->right));
    return result;
}

ref comb_l(stack& sl) {
    auto f = extract(sl);
    auto g = extract(sl);
    auto x = extract(sl);
    auto result = make_application(
        make_application(f->right, x->right),
        g->right);
    return result;
}

ref comb_r(stack& sl) {
    auto f = extract(sl);
    auto g = extract(sl);
    auto x = extract(sl);
    auto result = make_application(
        f->right,
        make_application(g->right, x->right));
    return result;
}

ref print(stack& sl) {
    auto arg = extract(sl);
    auto val = eval(arg->right);
    std::printf("%d\n", cast<number>(val)->value);
    return val;
}

ref update(application* app, ref newval) {
    if (auto* napp = try_cast<application>(newval)) {
        app->left = napp->left;
        app->right = napp->right;
    } else {
        app->left = make_function(comb_i);
        app->right = newval;
    }
    return app;
}

void print(ref r) {
    if (auto* app = try_cast<application>(r)) {
        printf("(");
        print(app->left);
        printf(" ");
        print(app->right);
        printf(")");
    } else if (auto* n = try_cast<number>(r)) {
        printf("%d", n->value);
    } else {
        printf("?");
    }
}

ref eval(ref r) {
    stack s;
    auto* orig = try_cast<application>(r);
    while (true) {
        while (auto* app = try_cast<application>(r)) {
            push(s, app);
            r = app->left;
        }

        assert(is<function>(r) && "type error: trying to apply non-func");
        auto* f = cast<function>(r);
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

int main() {
    {
        auto expr = make_application(
            make_function(print),
            make_application(
                make_application(
                    make_application(
                        make_function(comb_s),
                        make_application(
                            make_application(
                                make_function(comb_r),
                                make_function(add)),
                            make_function(comb_i))),
                    make_function(comb_i)),
                make_number(3)));
        eval(expr);
    }
    collect_garbage();
}
