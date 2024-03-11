#include <cassert>

#include "Value.hpp"
#include "User.hpp"

void Value::add_use(Value *val, unsigned arg_no) {
    use_list_.push_back(Use(val, arg_no));
}

void Value::replace_all_use_with(Value *new_val) {
    for (auto use : use_list_) {
        auto val = dynamic_cast<User *>(use.val_);
        assert(val && "exist a value(but not user) use old value");
        val->set_operand(use.arg_no_, new_val);
    }
    use_list_.clear();
}

void Value::replace_use_with_when(Value *new_val, std::function<bool(User *)> pred) {
    for (auto it = use_list_.begin(); it != use_list_.end();) {
        auto use = *it;
        auto val = dynamic_cast<User *>(use.val_);
        assert(val && "exist a value(but not user) use old value");
        if (not pred(val)) {
            ++it;
            continue;
        }
        val->set_operand(use.arg_no_, new_val);
        it = use_list_.erase(it);
    }
}

void Value::remove_use(Value *val) {
    auto is_val = [val](const Use &use) { return use.val_ == val; };
    use_list_.remove_if(is_val);
}

/*#include "Value.hpp"
#include "Type.hpp"
#include "User.hpp"

#include <cassert>

bool Value::set_name(std::string name) {
    if (name_ == "") {
        name_ = name;
        return true;
    }
    return false;
}

void Value::add_use(User *user, unsigned arg_no) {
    use_list_.emplace_back(user, arg_no);
};

void Value::remove_use(User *user, unsigned arg_no) {
    auto target_use = Use(user, arg_no);
    use_list_.remove_if([&](const Use &use) { return use == target_use; });
}

void Value::replace_all_use_with(Value *new_val) {
    if (this == new_val)
        return;
    while (use_list_.size()) {
        auto use = use_list_.begin();
        use->val_->set_operand(use->arg_no_, new_val);
    }
}

void Value::replace_use_with_if(Value *new_val,
                                std::function<bool(Use)> should_replace) {
    if (this == new_val)
        return;
    for (auto iter = use_list_.begin(); iter != use_list_.end();) {
        auto use = *iter++;
        if (not should_replace(use))
            continue;
        use.val_->set_operand(use.arg_no_, new_val);
    }
}
*/