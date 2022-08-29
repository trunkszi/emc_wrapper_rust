//
// Created by Administrator on 2022/8/22.
//

#include "strategy.h"
#include "wrapper.h"
#include <type_traits>
#include <cstring>
#include <cstdlib>
#include "wrapper/src/lib.rs.h"

namespace rust {
    bool is_include_chinese(const char *source) {
        char c;
        for (int i = 0; i < strlen(source) + 1; i++) {
            c = source[i];
            if (c == 0) {
                break;
            }
            if (c & 0x80) {
                if (source[i + 1] & 0x80) {
                    return true;
                }
            }
        }
        return false;
    }
    // 摘自 https://programmer.group/c-realizes-the-conversion-of-utf8-and-gbk-coded-strings.html
#ifdef _WIN32

#include <windows.h>

    std::string gbk_to_utf8(const char *src_str) {
        if (!is_include_chinese(src_str)) {
            return src_str;
        }
        int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
        wchar_t *wstr = new wchar_t[len + 1];
        memset(wstr, 0, len + 1);
        MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
        len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        char *str = new char[len + 1];
        memset(str, 0, len + 1);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
        std::string strTemp = str;
        if (wstr)
            delete[] wstr;
        if (str)
            delete[] str;
        return strTemp;
    }

    std::string utf8_to_gbk(const char *src_str) {
        int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
        wchar_t *wszGBK = new wchar_t[len + 1];
        memset(wszGBK, 0, len * 2 + 2);
        MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
        len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
        char *szGBK = new char[len + 1];
        memset(szGBK, 0, len + 1);
        WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
        std::string strTemp(szGBK);
        if (wszGBK)
            delete[] wszGBK;
        if (szGBK)
            delete[] szGBK;
        return strTemp;
    }

#else

#include <iconv.h>

    int GbkToUtf8(char *str_str, size_t src_len, char *dst_str, size_t dst_len) {
        iconv_t cd;
        char **pin = &str_str;
        char **pout = &dst_str;

        cd = iconv_open("utf8", "gbk");
        if (cd == 0)
            return -1;
        memset(dst_str, 0, dst_len);
        if (iconv(cd, pin, &src_len, pout, &dst_len) == -1)
            return -1;
        iconv_close(cd);
        *pout = '\0';

        return 0;
    }

#endif

    //    std::string format_time(double duration) {
    //        std::chrono::duration<double> milliseconds = std::chrono::duration<double>(duration);
    //        auto sec = std::chrono::duration_cast<std::chrono::seconds>(milliseconds);
    //
    //        auto tp = std::chrono::time_point<std::chrono::system_clock>(sec); // 将秒数转换为时间点
    //        std::time_t tm = std::chrono::system_clock::to_time_t(tp); // 将时间点转换为时间戳
    //        std::stringstream ss;
    //        ss << std::put_time(std::localtime(&tm), "%F %T");
    //        return std::move(ss.str());
    //    }

    rust::OnOrder *order_to_on_order(Order &order) {
        rust::OnOrder *o = new rust::OnOrder;
        o->strategy_id = gbk_to_utf8(order.strategy_id);
        o->account_id = gbk_to_utf8(order.account_id);
        o->account_name = gbk_to_utf8(order.account_name);
        o->cl_ord_id = gbk_to_utf8(order.cl_ord_id);
        o->order_id = gbk_to_utf8(order.order_id);
        o->ex_ord_id = gbk_to_utf8(order.ex_ord_id);
        o->algo_order_id = order.algo_order_id;
        o->order_business = order.order_business;
        o->symbol = gbk_to_utf8(order.symbol);
        o->side = order.side;
        o->position_effect = order.position_effect;
        o->position_side = order.position_side;
        o->order_type = order.order_type;
        o->order_duration = order.order_duration;
        o->order_qualifier = order.order_qualifier;
        o->order_src = order.order_src;
        o->position_src = order.position_src;
        o->status = order.status;
        o->ord_rej_reason = order.ord_rej_reason;
        o->ord_rej_reason_detail = gbk_to_utf8(order.ord_rej_reason_detail);
        o->price = order.price;
        o->stop_price = order.stop_price;
        o->order_style = order.order_style;
        o->volume = order.volume;
        o->value = order.value;
        o->percent = order.percent;
        o->target_volume = order.target_volume;
        o->target_value = order.target_value;
        o->target_percent = order.target_percent;
        o->filled_volume = order.filled_volume;
        o->filled_vwap = order.filled_vwap;
        o->filled_amount = order.filled_amount;
        o->filled_commission = order.filled_commission;
        o->created_at = order.created_at;
        o->updated_at = order.updated_at;
        return o;
    }

    class strategy::impl : public Strategy, public std::enable_shared_from_this<strategy::impl> {
    private:
        strategy *_strategy = nullptr;
        CallbackImpl *_callback = nullptr;

    public:
        impl(strategy *s, CallbackImpl *callback) : _strategy(s), _callback(callback) {
        }

        ~impl() {
        }

        std::shared_ptr <strategy::impl> ptr() {
            return shared_from_this();
        }

        //初始化完成
        void on_init() override {
            _callback->on_init();
        }

        //收到Tick行情
        void on_tick(Tick *tick) override {
            if (tick == nullptr) {
                return;
            }

            rust::OnTick *t = new rust::OnTick;
            memset(t, 0, sizeof(rust::OnTick));
            t->symbol = gbk_to_utf8(tick->symbol);
            t->created_at = tick->created_at;
            t->price = tick->price;
            t->open = tick->open;
            t->high = tick->high;
            t->low = tick->low;
            t->cum_volume = tick->cum_volume;
            t->cum_amount = tick->cum_amount;
            t->cum_position = tick->cum_position;
            t->last_amount = tick->last_amount;
            t->last_volume = tick->last_volume;
            t->trade_type = tick->trade_type;
            for (int i = 0; i < DEPTH_OF_QUOTE; i++) {
                rust::OnQuote q;
                q.bid_price = tick->quotes[i].bid_price;
                q.bid_volume = tick->quotes[i].bid_volume;
                q.ask_price = tick->quotes[i].ask_price;
                q.ask_volume = tick->quotes[i].ask_volume;
                t->quotes.push_back(q);
            }
            _callback->on_tick(rust::Box<OnTick>::from_raw(t));
        }

        //收到bar行情
        void on_bar(Bar *bar) override {
            if (bar == nullptr) {
                return;
            }

            rust::OnBar *b = new rust::OnBar;
            memset(b, 0, sizeof(rust::OnBar));
            b->symbol = gbk_to_utf8(bar->symbol);
            b->bob = (bar->bob);
            b->eob = (bar->eob);
            b->open = bar->open;
            b->close = bar->close;
            b->high = bar->high;
            b->low = bar->low;
            b->volume = bar->volume;
            b->amount = bar->amount;
            b->pre_close = bar->pre_close;
            b->position = bar->position;;
            b->frequency = gbk_to_utf8(bar->frequency);
            _callback->on_bar(rust::Box<rust::OnBar>::from_raw(b));
        }

        //收到逐笔成交（L2行情时有效）
        virtual void on_l2transaction(L2Transaction *l2transaction) override {
            if (l2transaction == nullptr) {
                return;
            }

            rust::OnL2Transaction *l = new rust::OnL2Transaction;
            memset(l, 0, sizeof(rust::OnL2Transaction));
            l->symbol = gbk_to_utf8(l2transaction->symbol);
            l->created_at = (l2transaction->created_at);
            l->price = l2transaction->price;
            l->volume = l2transaction->volume;
            l->side = l2transaction->side;
            l->exec_type = l2transaction->exec_type;
            l->exec_index = l2transaction->exec_index;
            l->ask_order_index = l2transaction->ask_order_index;
            l->bid_order_index = l2transaction->bid_order_index;
            _callback->on_l2transaction(rust::Box<rust::OnL2Transaction>::from_raw(l));
        }

        //收到逐笔委托（深交所L2行情时有效）
        virtual void on_l2order(L2Order *l2order) override {
            if (l2order == nullptr) {
                return;
            }
            rust::OnL2Order *o = new rust::OnL2Order;
            memset(o, 0, sizeof(rust::OnL2Order));
            o->symbol = gbk_to_utf8(l2order->symbol);
            o->created_at = (l2order->created_at);
            o->price = l2order->price;
            o->volume = l2order->volume;
            o->side = l2order->side;
            o->order_type = l2order->order_type;
            o->order_index = l2order->order_index;
            _callback->on_l2order(rust::Box<rust::OnL2Order>::from_raw(o));
        }

        //收到委托队列（上交所L2行情时有效）
        virtual void on_l2order_queue(L2OrderQueue *l2queue) override {
            if (l2queue == nullptr) {
                return;
            }
            rust::OnL2OrderQueue *q = new rust::OnL2OrderQueue;
            memset(q, 0, sizeof(rust::OnL2OrderQueue));
            q->symbol = gbk_to_utf8(l2queue->symbol);
            q->created_at = (l2queue->created_at);
            q->price = l2queue->price;
            q->volume = l2queue->volume;
            q->side = l2queue->side;
            q->queue_orders = l2queue->queue_orders;
            for (int i = 0; i < l2queue->queue_orders; i++) {
                q->queue_volumes.push_back(l2queue->queue_volumes[i]);
            }
            _callback->on_l2order_queue(rust::Box<rust::OnL2OrderQueue>::from_raw(q));
        }

        //委托变化
        virtual void on_order_status(Order *order) override {
            if (order == nullptr) {
                return;
            }
            rust::OnOrder *o = new rust::OnOrder;
            o->strategy_id = gbk_to_utf8(order->strategy_id);
            o->account_id = gbk_to_utf8(order->account_id);
            o->account_name = gbk_to_utf8(order->account_name);
            o->cl_ord_id = gbk_to_utf8(order->cl_ord_id);
            o->order_id = gbk_to_utf8(order->order_id);
            o->ex_ord_id = gbk_to_utf8(order->ex_ord_id);
            o->algo_order_id = order->algo_order_id;
            o->order_business = order->order_business;

            o->symbol = gbk_to_utf8(order->symbol);
            o->side = order->side;
            o->position_effect = order->position_effect;
            o->position_side = order->position_side;

            o->order_type = order->order_type;
            o->order_duration = order->order_duration;
            o->order_qualifier = order->order_qualifier;
            o->order_src = order->order_src;
            o->position_src = order->position_src;

            o->status = order->status;
            o->ord_rej_reason = order->ord_rej_reason;
            o->ord_rej_reason_detail = gbk_to_utf8(order->ord_rej_reason_detail);

            o->price = order->price;
            o->stop_price = order->stop_price;

            o->order_style = order->order_style;
            o->volume = order->volume;
            o->value = order->value;
            o->percent = order->percent;
            o->target_volume = order->target_volume;
            o->target_value = order->target_value;
            o->target_percent = order->target_percent;

            o->filled_volume = order->filled_volume;
            o->filled_vwap = order->filled_vwap;
            o->filled_amount = order->filled_amount;
            o->filled_commission = order->filled_commission;

            o->created_at = (order->created_at);
            o->updated_at = (order->updated_at);
            _callback->on_order_status(rust::Box<rust::OnOrder>::from_raw(o));
        }

        //执行回报
        virtual void on_execution_report(ExecRpt *rpt) override {
            if (rpt == nullptr) {
                return;
            }
            rust::OnExecRpt *e = new rust::OnExecRpt;
            e->strategy_id = gbk_to_utf8(rpt->strategy_id);
            e->account_id = gbk_to_utf8(rpt->account_id);
            e->account_name = gbk_to_utf8(rpt->account_name);

            e->cl_ord_id = gbk_to_utf8(rpt->cl_ord_id);
            e->order_id = gbk_to_utf8(rpt->order_id);
            e->exec_id = gbk_to_utf8(rpt->exec_id);

            e->symbol = gbk_to_utf8(rpt->symbol);

            e->position_effect = rpt->position_effect;
            e->side = rpt->side;
            e->ord_rej_reason = rpt->ord_rej_reason;
            e->ord_rej_reason_detail = gbk_to_utf8(rpt->ord_rej_reason_detail);
            e->exec_type = rpt->exec_type;

            e->price = rpt->price;
            e->volume = rpt->volume;
            e->amount = rpt->amount;
            e->commission = rpt->commission;
            e->cost = rpt->cost;
            e->created_at = (rpt->created_at);
            _callback->on_execution_report(rust::Box<rust::OnExecRpt>::from_raw(e));
        }

        //算法委托变化
        virtual void on_algo_order_status(AlgoOrder *order) override {
            if (order == nullptr) {
                return;
            }
            rust::OnAlgoOrder *o = new rust::OnAlgoOrder;
            o->strategy_id = gbk_to_utf8(order->strategy_id);
            o->account_id = gbk_to_utf8(order->account_id);
            o->account_name = gbk_to_utf8(order->account_name);

            o->cl_ord_id = gbk_to_utf8(order->cl_ord_id);
            o->order_id = gbk_to_utf8(order->order_id);
            o->ex_ord_id = gbk_to_utf8(order->ex_ord_id);
            o->order_business = order->order_business;

            o->symbol = gbk_to_utf8(order->symbol);
            o->side = order->side;
            o->position_effect = order->position_effect;
            o->position_side = order->position_side;

            o->order_type = order->order_type;
            o->order_duration = order->order_duration;
            o->order_qualifier = order->order_qualifier;
            o->order_src = order->order_src;
            o->position_src = order->position_src;

            o->status = order->status;
            o->ord_rej_reason = order->ord_rej_reason;
            o->ord_rej_reason_detail = gbk_to_utf8(order->ord_rej_reason_detail);

            o->price = order->price;
            o->stop_price = order->stop_price;

            o->order_style = order->order_style;
            o->volume = order->volume;
            o->value = order->value;
            o->percent = order->percent;
            o->target_volume = order->target_volume;
            o->target_value = order->target_value;
            o->target_percent = order->target_percent;

            o->filled_volume = order->filled_volume;
            o->filled_vwap = order->filled_vwap;
            o->filled_amount = order->filled_amount;
            o->filled_commission = order->filled_commission;

            o->algo_name = gbk_to_utf8(order->algo_name);
            o->algo_param = gbk_to_utf8(order->algo_param);
            o->algo_status = order->algo_status;
            o->algo_comment = gbk_to_utf8(order->algo_comment);

            o->created_at = (order->created_at);
            o->updated_at = (order->updated_at);

            _callback->on_algo_order_status(rust::Box<rust::OnAlgoOrder>::from_raw(o));
        }

        //资金推送
        virtual void on_cash(Cash *cash) override {
            if (cash == nullptr) {
                return;
            }
            rust::OnCash *c = new rust::OnCash;
            c->account_id = gbk_to_utf8(cash->account_id);
            c->account_name = gbk_to_utf8(cash->account_name);
            c->currency = cash->currency;

            c->nav = cash->nav;
            c->pnl = cash->pnl;
            c->fpnl = cash->fpnl;
            c->frozen = cash->frozen;
            c->order_frozen = cash->order_frozen;
            c->available = cash->available;
            c->balance = cash->balance;
            c->market_value = cash->market_value;
            c->cum_inout = cash->cum_inout;
            c->cum_trade = cash->cum_trade;
            c->cum_pnl = cash->cum_pnl;
            c->cum_commission = cash->cum_commission;

            c->last_trade = cash->last_trade;
            c->last_pnl = cash->last_pnl;
            c->last_commission = cash->last_commission;
            c->last_inout = cash->last_inout;
            c->change_reason = cash->change_reason;
            c->change_event_id = gbk_to_utf8(cash->change_event_id);

            c->created_at = (cash->created_at);
            c->updated_at = (cash->updated_at);
            _callback->on_cash(rust::Box<rust::OnCash>::from_raw(c));
        }

        //持仓推送
        virtual void on_position(Position *position) override {
            if (position == nullptr) {
                return;
            }
            rust::OnPosition *p = new rust::OnPosition;
            p->account_id = gbk_to_utf8(position->account_id);
            p->account_name = gbk_to_utf8(position->account_name);

            p->symbol = gbk_to_utf8(position->symbol);
            p->side = position->side;
            p->volume = position->volume;
            p->volume_today = position->volume_today;
            p->vwap = position->vwap;
            p->vwap_diluted = position->vwap_diluted;
            p->vwap_open = position->vwap_open;
            p->amount = position->amount;

            p->price = position->price;
            p->fpnl = position->fpnl;
            p->fpnl_open = position->fpnl_open;
            p->cost = position->cost;
            p->order_frozen = position->order_frozen;
            p->order_frozen_today = position->order_frozen_today;
            p->available = position->available;
            p->available_today = position->available_today;
            p->available_now = position->available_now;
            p->market_value = position->market_value;

            p->last_price = position->last_price;
            p->last_volume = position->last_volume;
            p->last_inout = position->last_inout;
            p->change_reason = position->change_reason;
            p->change_event_id = gbk_to_utf8(position->change_event_id);

            p->has_dividend = position->has_dividend;
            p->created_at = (position->created_at);
            p->updated_at = (position->updated_at);
            _callback->on_position(rust::Box<rust::OnPosition>::from_raw(p));
        }

        //参数变化
        virtual void on_parameter(Parameter *param) override {
            if (param == nullptr) {
                return;
            }
            rust::OnParameter *p = new rust::OnParameter;
            p->key = gbk_to_utf8(param->key);
            p->value = param->value;
            p->min = param->min;
            p->max = param->max;
            p->name = gbk_to_utf8(param->name);
            p->intro = gbk_to_utf8(param->intro);
            p->group = gbk_to_utf8(param->group);
            p->readonly = param->readonly;
            _callback->on_parameter(rust::Box<rust::OnParameter>::from_raw(p));
        }

        //定时任务触发
        virtual void on_schedule(const char *data_rule, const char *time_rule) override {
            _callback->on_schedule(gbk_to_utf8(data_rule), gbk_to_utf8(time_rule));
        }

        //回测完成后收到绩效报告
        virtual void on_backtest_finished(Indicator *indicator) override {
            if (indicator == nullptr) {
                return;
            }
            rust::OnIndicator *i = new rust::OnIndicator;
            i->account_id = gbk_to_utf8(indicator->account_id);
            i->pnl_ratio = indicator->pnl_ratio;
            i->pnl_ratio_annual = indicator->pnl_ratio_annual;
            i->sharp_ratio = indicator->sharp_ratio;
            i->max_drawdown = indicator->max_drawdown;
            i->risk_ratio = indicator->risk_ratio;
            i->open_count = indicator->open_count;
            i->close_count = indicator->close_count;
            i->win_count = indicator->win_count;
            i->lose_count = indicator->lose_count;
            i->win_ratio = indicator->win_ratio;
            i->created_at = (indicator->created_at);
            i->updated_at = (indicator->updated_at);
            _callback->on_backtest_finished(rust::Box<rust::OnIndicator>::from_raw(i));
        }

        //实盘账号状态变化
        virtual void on_account_status(AccountStatus *account_status) override {
            if (account_status == nullptr) {
                return;
            }
            rust::OnAccountStatus *s = new rust::OnAccountStatus;

            s->account_id = gbk_to_utf8(account_status->account_id);
            s->account_name = gbk_to_utf8(account_status->account_name);
            s->state = account_status->state;
            s->error_code = account_status->error_code;
            s->error_msg = gbk_to_utf8(account_status->error_msg);

            _callback->on_account_status(rust::Box<rust::OnAccountStatus>::from_raw(s));
        }

        //错误产生
        virtual void on_error(int error_code, const char *error_msg) override {
            char buffer[32]{0};
            itoa(error_code, buffer, 10);
            _callback->on_error(buffer, gbk_to_utf8(error_msg));
        }

        //收到策略停止信号
        virtual void on_stop() override {
            _callback->on_stop();
        }

        //数据已经连接上
        virtual void on_market_data_connected() override {
            _callback->on_market_data_connected();
        }

        //交易已经连接上
        virtual void on_trade_data_connected() override {
            _callback->on_trade_data_connected();
        }

        //数据连接断开了
        virtual void on_market_data_disconnected() override {
            _callback->on_market_data_disconnected();
        }

        //交易连接断开了
        virtual void on_trade_data_disconnected() override {
            _callback->on_trade_data_disconnected();
        }
    };

    strategy::strategy(rust::CallbackImpl *callback) {
        _impl = new strategy::impl(this, callback);
    }

    strategy::~strategy() {
        if (_impl != nullptr) {
            delete _impl;
            _impl = nullptr;
        }
    }

    std::shared_ptr <strategy> strategy::ptr() {
        return shared_from_this();
    }

    void strategy::set_strategy_id(rust::String &strategy_id) const {
        _impl->set_strategy_id(strategy_id.c_str());
    }

    void strategy::set_token(rust::String &token) const {
        _impl->set_token(token.c_str());
    }

    void strategy::set_mode(int mode) const {
        _impl->set_mode(mode);
    }

    int strategy::schedule(rust::String &data_rule, rust::String &time_rule) const {
        return _impl->schedule(data_rule.c_str(), time_rule.c_str());
    }

    int strategy::set_backtest_config(rust::String &start_time, rust::String &end_time, double initial_cash,
                                      double transaction_ratio, double commission_ratio, double slippage_ratio,
                                      int adjust,
                                      int check_cache) const {
        return _impl->set_backtest_config(start_time.c_str(), end_time.c_str(), initial_cash, transaction_ratio,
                                          commission_ratio, slippage_ratio, adjust, check_cache);
    }

    int strategy::subscribe(rust::String &symbols, rust::String &frequency, bool unsubscribe_previous) const {
        return _impl->subscribe(symbols.c_str(), frequency.c_str(), unsubscribe_previous);
    }

    int strategy::unsubscribe(rust::String &symbols, rust::String &frequency) const {
        return _impl->unsubscribe(symbols.c_str(), frequency.c_str());
    }

    rust::Vec<OnAccount> strategy::get_accounts() const {
        rust::Vec<OnAccount> v;
        auto array = _impl->get_accounts();
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }
        for (int i = 0; i < array->count(); ++i) {
            auto a = array->at(i);
            rust::OnAccount oa;
            oa.account_id = gbk_to_utf8(a.account_id);
            oa.account_name = gbk_to_utf8(a.account_name);
            oa.title = gbk_to_utf8(a.title);
            oa.intro = gbk_to_utf8(a.intro);
            oa.comment = gbk_to_utf8(a.comment);
            v.emplace_back(std::move(oa));
        }
        array->release();
        return std::move(v);
    }

    int strategy::run() const {
        return _impl->run();
    }

    void strategy::stop() const {
        _impl->stop();
    }

    double strategy::now() const {
        return _impl->now();
    }

    rust::Box<OnAccountStatus> strategy::get_account_status(rust::String &account) const {
        AccountStatus as;
        auto result = _impl->get_account_status(account.c_str(), as);
        if (result != 0) {
            return rust::Box<OnAccountStatus>::from_raw(nullptr);
        }
        OnAccountStatus *os = new OnAccountStatus;
        os->account_id = gbk_to_utf8(as.account_id);
        os->account_name = gbk_to_utf8(as.account_name);
        os->state = as.state;
        os->error_code = as.error_code;
        os->error_msg = gbk_to_utf8(as.error_msg);
        return rust::Box<OnAccountStatus>::from_raw(os);
    }

    rust::Vec<OnAccountStatus> strategy::get_all_account_status() const {
        rust::Vec<OnAccountStatus> v;
        auto array = _impl->get_all_account_status();
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }
        for (int i = 0; i < array->count(); ++i) {
            auto a = array->at(i);
            rust::OnAccountStatus oa;
            oa.account_id = gbk_to_utf8(a.account_id);
            oa.account_name = gbk_to_utf8(a.account_name);
            oa.state = a.state;
            oa.error_code = a.error_code;
            oa.error_msg = gbk_to_utf8(a.error_msg);
            v.emplace_back(std::move(oa));
        }
        array->release();
        return std::move(v);
    }

    Box <OnOrder>
    strategy::order_volume(rust::String &symbol, int volume, int side, int order_type, int position_effect,
                           double price,
                           rust::String &account) const {
        auto o = _impl->order_volume(symbol.c_str(), volume, side, order_type, position_effect, price,
                                     strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    Box <OnOrder>
    strategy::order_value(String &symbol, double value, int side, int order_type, int position_effect, double price,
                          String &account) const {
        auto o = _impl->order_value(symbol.c_str(), value, side, order_type, position_effect, price,
                                    strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    Box <OnOrder>
    strategy::order_percent(String &symbol, double percent, int side, int order_type, int position_effect, double price,
                            String &account) const {
        auto o = _impl->order_percent(symbol.c_str(), percent, side, order_type, position_effect, price,
                                      strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    Box <OnOrder>
    strategy::order_target_volume(String &symbol, int volume, int side, int order_type,
                                  double price, String &account) const {
        auto o = _impl->order_target_volume(symbol.c_str(), volume, side, order_type, price,
                                            strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    Box <OnOrder>
    strategy::order_target_value(String &symbol, double value, int position_side, int order_type,
                                 double price, String &account) const {
        auto o = _impl->order_target_value(symbol.c_str(), value, position_side, order_type, price,
                                           strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    Box <OnOrder> strategy::order_target_percent(String &symbol, double percent, int position_side, int order_type,
                                                 double price, String &account) const {
        auto o = _impl->order_target_percent(symbol.c_str(), percent, position_side, order_type, price,
                                             strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    rust::Vec<OnOrder> strategy::order_close_all() const {
        rust::Vec<OnOrder> v;
        auto array = _impl->order_close_all();
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }
        for (int i = 0; i < array->count(); ++i) {
            auto o = array->at(i);
            auto ptr = order_to_on_order(o);
            v.push_back(*ptr);
            delete ptr;
        }
        array->release();
        return std::move(v);
    }

    int strategy::order_cancel(String &cl_ord_id, String &account) const {
        return _impl->order_cancel(cl_ord_id.c_str(), strlen(account.c_str()) == 0 ? nullptr : account.c_str());
    }

    int strategy::order_cancel_all() const {
        return _impl->order_cancel_all();
    }

    Box <OnOrder>
    strategy::place_order(rust::String &symbol, int volume, int side, int order_type, int position_effect, double price,
                          int order_duration, int order_qualifier, double stop_price, int order_business,
                          rust::String &account) const {
        auto o = _impl->place_order(symbol.c_str(), volume, side, order_type, position_effect, price, order_duration,
                                    order_qualifier, stop_price, order_business,
                                    strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    Box <OnOrder>
    strategy::order_after_hour(rust::String &symbol, int volume, int side, double price, rust::String &account) const {
        auto o = _impl->order_after_hour(symbol.c_str(), volume, side, price,
                                         strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    rust::Vec<OnOrder> strategy::get_orders(rust::String &account) const {
        rust::Vec<OnOrder> v;
        auto array = _impl->get_orders(strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }
        for (int i = 0; i < array->count(); ++i) {
            auto o = array->at(i);
            auto ptr = order_to_on_order(o);
            v.push_back(*ptr);
            delete ptr;
        }
        array->release();
        return std::move(v);
    }

    rust::Vec<OnOrder> strategy::get_unfinished_orders(rust::String &account) const {
        rust::Vec<OnOrder> v;
        auto array = _impl->get_unfinished_orders(strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }
        for (int i = 0; i < array->count(); ++i) {
            auto o = array->at(i);
            auto ptr = order_to_on_order(o);
            v.push_back(*ptr);
            delete ptr;
        }
        array->release();
        return std::move(v);
    }

    rust::Vec<OnExecRpt> strategy::get_execution_reports(rust::String &account) const {
        rust::Vec<OnExecRpt> v;
        auto array = _impl->get_execution_reports(strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }
        for (int i = 0; i < array->count(); ++i) {
            auto rpt = array->at(i);
            OnExecRpt r;
            r.strategy_id = gbk_to_utf8(rpt.strategy_id);
            r.account_id = gbk_to_utf8(rpt.account_id);
            r.account_name = gbk_to_utf8(rpt.account_name);
            r.cl_ord_id = gbk_to_utf8(rpt.cl_ord_id);
            r.order_id = gbk_to_utf8(rpt.order_id);
            r.exec_id = gbk_to_utf8(rpt.exec_id);
            r.symbol = gbk_to_utf8(rpt.symbol);
            r.position_effect = rpt.position_effect;
            r.side = rpt.side;
            r.ord_rej_reason = rpt.ord_rej_reason;
            r.ord_rej_reason_detail = gbk_to_utf8(rpt.ord_rej_reason_detail);
            r.exec_type = rpt.exec_type;
            r.price = rpt.price;
            r.volume = rpt.volume;
            r.amount = rpt.amount;
            r.commission = rpt.commission;
            r.cost = rpt.cost;
            r.created_at = rpt.created_at;
            v.emplace_back(std::move(r));
        }
        array->release();
        return std::move(v);
    }

    rust::Vec<rust::OnCash> strategy::get_cash(rust::String &account) const {
        rust::Vec<rust::OnCash> v;
        auto array = _impl->get_cash(strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }
        for (int i = 0; i < array->count(); ++i) {
            auto cash = array->at(i);
            rust::OnCash c;
            c.account_id = gbk_to_utf8(cash.account_id);
            c.account_name = gbk_to_utf8(cash.account_name);
            c.currency = cash.currency;
            c.nav = cash.nav;
            c.pnl = cash.pnl;
            c.fpnl = cash.fpnl;
            c.frozen = cash.frozen;
            c.order_frozen = cash.order_frozen;
            c.available = cash.available;
            c.balance = cash.balance;
            c.market_value = cash.market_value;
            c.cum_inout = cash.cum_inout;
            c.cum_trade = cash.cum_trade;
            c.cum_pnl = cash.cum_pnl;
            c.cum_commission = cash.cum_commission;
            c.last_trade = cash.last_trade;
            c.last_pnl = cash.last_pnl;
            c.last_commission = cash.last_commission;
            c.last_inout = cash.last_inout;
            c.change_reason = cash.change_reason;
            c.change_event_id = gbk_to_utf8(cash.change_event_id);
            c.created_at = cash.created_at;
            c.updated_at = cash.updated_at;
            v.push_back(c);
        }
        array->release();
        return v;
    }

    rust::Vec<OnPosition> strategy::get_position(rust::String &account) const {
        rust::Vec<OnPosition> v;
        auto array = _impl->get_position(strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }
        for (int i = 0; i < array->count(); ++i) {
            auto pos = array->at(i);
            OnPosition p;
            p.account_id = gbk_to_utf8(pos.account_id);
            p.account_name = gbk_to_utf8(pos.account_name);

            p.symbol = gbk_to_utf8(pos.symbol);
            p.side = pos.side;
            p.volume = pos.volume;
            p.volume_today = pos.volume_today;
            p.vwap = pos.vwap;
            p.vwap_diluted = pos.vwap_diluted;
            p.vwap_open = pos.vwap_open;
            p.amount = pos.amount;

            p.price = pos.price;
            p.fpnl = pos.fpnl;
            p.fpnl_open = pos.fpnl_open;
            p.cost = pos.cost;
            p.order_frozen = pos.order_frozen;
            p.order_frozen_today = pos.order_frozen_today;
            p.available = pos.available;
            p.available_today = pos.available_today;
            p.available_now = pos.available_now;
            p.market_value = pos.market_value;

            p.last_price = pos.last_price;
            p.last_volume = pos.last_volume;
            p.last_inout = pos.last_inout;
            p.change_reason = pos.change_reason;
            p.change_event_id = gbk_to_utf8(pos.change_event_id);

            p.has_dividend = pos.has_dividend;
            p.created_at = (pos.created_at);
            p.updated_at = (pos.updated_at);
            v.emplace_back(std::move(p));
        }
        array->release();
        return std::move(v);
    }

    rust::Box<OnAlgoOrder>
    strategy::order_algo(rust::String &symbol, int volume, int position_effect, int side, int order_type, double price,
                         rust::String &algo_name, rust::String &algo_param, rust::String &account) const {
        auto order = _impl->order_algo(symbol.c_str(), volume, position_effect, side, order_type, price,
                                       algo_name.c_str(),
                                       algo_param.c_str(),
                                       strlen(account.c_str()) == 0 ? nullptr : account.c_str());

        OnAlgoOrder *o = new OnAlgoOrder;
        o->strategy_id = gbk_to_utf8(order.strategy_id);
        o->account_id = gbk_to_utf8(order.account_id);
        o->account_name = gbk_to_utf8(order.account_name);
        o->cl_ord_id = gbk_to_utf8(order.cl_ord_id);
        o->order_id = gbk_to_utf8(order.order_id);
        o->ex_ord_id = gbk_to_utf8(order.ex_ord_id);
        o->order_business = order.order_business;
        o->symbol = gbk_to_utf8(order.symbol);
        o->side = order.side;
        o->position_effect = order.position_effect;
        o->position_side = order.position_side;
        o->order_type = order.order_type;
        o->order_duration = order.order_duration;
        o->order_qualifier = order.order_qualifier;
        o->order_src = order.order_src;
        o->position_src = order.position_src;
        o->status = order.status;
        o->ord_rej_reason = order.ord_rej_reason;
        o->ord_rej_reason_detail = gbk_to_utf8(order.ord_rej_reason_detail);
        o->price = order.price;
        o->stop_price = order.stop_price;
        o->order_style = order.order_style;
        o->volume = order.volume;
        o->value = order.value;
        o->percent = order.percent;
        o->target_volume = order.target_volume;
        o->target_value = order.target_value;
        o->target_percent = order.target_percent;
        o->filled_volume = order.filled_volume;
        o->filled_vwap = order.filled_vwap;
        o->filled_amount = order.filled_amount;
        o->filled_commission = order.filled_commission;
        o->algo_name = gbk_to_utf8(order.algo_name);
        o->algo_param = gbk_to_utf8(order.algo_param);
        o->algo_status = order.algo_status;
        o->algo_comment = gbk_to_utf8(order.algo_comment);
        o->created_at = (order.created_at);
        o->updated_at = (order.updated_at);
        return rust::Box<OnAlgoOrder>::from_raw(o);
    }

    int strategy::algo_order_cancel(rust::String &cl_ord_id, rust::String &account) const {
        return _impl->algo_order_cancel(cl_ord_id.c_str(), strlen(account.c_str()) == 0 ? nullptr : account.c_str());
    }

    rust::Vec<OnOrder> strategy::get_algo_child_orders(rust::String &cl_ord_id, rust::String &account) const {
        rust::Vec<OnOrder> v;
        auto array = _impl->get_algo_child_orders(cl_ord_id.c_str(),
                                                  strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }
        for (int i = 0; i < array->count(); ++i) {
            auto order = array->at(i);
            auto ptr = order_to_on_order(order);
            v.push_back(std::move(*ptr));
            delete ptr;
        }
        array->release();
        return std::move(v);
    }

    rust::String strategy::raw_func(rust::String &account, rust::String &func_id, rust::String &func_args) const {
        char *ptr = new char[1024];
        memset(ptr, 0, 1024);
        auto result = _impl->raw_func(account.c_str(), func_id.c_str(), func_args.c_str(), ptr);
        if (result != 0) {
            return rust::String();
        }
        rust::String r(ptr);
        return std::move(r);
    }

    rust::Box<OnOrder>
    strategy::credit_buying_on_margin(int position_src, String &symbol, int volume, double price, int order_type,
                                      int order_duration, int order_qualifier, String &account) const {
        auto order = _impl->credit_buying_on_margin(position_src, symbol.c_str(), volume, price, order_type,
                                                    order_duration, order_qualifier,
                                                    strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(order);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    rust::Box<OnOrder>
    strategy::credit_short_selling(int position_src, String &symbol, int volume, double price, int order_type,
                                   int order_duration, int order_qualifier, String &account) const {
        auto order = _impl->credit_short_selling(position_src, symbol.c_str(), volume, price, order_type,
                                                 order_duration,
                                                 order_qualifier,
                                                 strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(order);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    rust::Box<OnOrder>
    strategy::credit_repay_share_by_buying_share(String &symbol, int volume, double price, int order_type,
                                                 int order_duration, int order_qualifier,
                                                 String &account) const {
        auto order = _impl->credit_repay_share_by_buying_share(symbol.c_str(), volume, price, order_type,
                                                               order_duration,
                                                               order_qualifier,
                                                               strlen(account.c_str()) == 0 ? nullptr
                                                                                            : account.c_str());
        auto ptr = order_to_on_order(order);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    rust::Box<OnOrder>
    strategy::credit_repay_cash_by_selling_share(String &symbol, int volume, double price, int order_type,
                                                 int order_duration, int order_qualifier,
                                                 String &account) const {
        auto order = _impl->credit_repay_cash_by_selling_share(symbol.c_str(), volume, price, order_type,
                                                               order_duration,
                                                               order_qualifier,
                                                               strlen(account.c_str()) == 0 ? nullptr
                                                                                            : account.c_str());
        auto ptr = order_to_on_order(order);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    rust::Box<OnOrder>
    strategy::credit_buying_on_collateral(String &symbol, int volume, double price, int order_type, int order_duration,
                                          int order_qualifier, String &account) const {
        auto order = _impl->credit_buying_on_collateral(symbol.c_str(), volume, price, order_type, order_duration,
                                                        order_qualifier,
                                                        strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(order);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    rust::Box<OnOrder>
    strategy::credit_selling_on_collateral(String &symbol, int volume, double price, int order_type, int order_duration,
                                           int order_qualifier, String &account) const {
        auto order = _impl->credit_selling_on_collateral(symbol.c_str(), volume, price, order_type, order_duration,
                                                         order_qualifier,
                                                         strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(order);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    rust::Box<OnOrder> strategy::credit_repay_share_directly(String &symbol, int volume, String &account) const {
        auto order = _impl->credit_repay_share_directly(symbol.c_str(), volume,
                                                        strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(order);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    rust::Box<rust::RepayCashParam> strategy::credit_repay_cash_directly(double amount, String &account) const {
        char *ptr = nullptr;
        int buf_len = 0;
        double actual_repay_amount = 0.0;
        rust::RepayCashParam *repay = new rust::RepayCashParam;
        auto result = _impl->credit_repay_cash_directly(amount,
                                                        strlen(account.c_str()) == 0 ? nullptr : account.c_str(),
                                                        &actual_repay_amount, ptr, buf_len);

        repay->actual_repay_amount = actual_repay_amount;
        if (ptr != nullptr) {
            repay->error_msg_buf = std::move(rust::String(ptr, buf_len));
            free(ptr);
        } else {
            repay->error_msg_buf = std::move(rust::String());
        }
        repay->error_code = result;
        return rust::Box<rust::RepayCashParam>::from_raw(repay);
    }

    rust::Box<OnOrder> strategy::credit_collateral_in(String &symbol, int volume, String &account) const {
        auto order = _impl->credit_collateral_in(symbol.c_str(), volume,
                                                 strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(order);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    rust::Box<OnOrder> strategy::credit_collateral_out(String &symbol, int volume, String &account) const {
        auto order = _impl->credit_collateral_out(symbol.c_str(), volume,
                                                  strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto ptr = order_to_on_order(order);
        return rust::Box<OnOrder>::from_raw(ptr);
    }

    rust::Vec<OnCollateralInstrument>
    strategy::credit_get_collateral_instruments(rust::String &account) const {
        rust::Vec<OnCollateralInstrument> v;
        auto array = _impl->credit_get_collateral_instruments(strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }

        for (int i = 0; i < array->count(); i++) {
            auto in = array->at(i);
            OnCollateralInstrument instrument;
            instrument.symbol = gbk_to_utf8(in.symbol);
            instrument.name = gbk_to_utf8(in.name);
            instrument.pledge_rate = in.pledge_rate;
            v.emplace_back(std::move(instrument));
        }
        array->release();
        return std::move(v);
    }

    rust::Vec<OnBorrowableInstrument>
    strategy::credit_get_borrowable_instruments(int position_src, String &account) const {
        rust::Vec<OnBorrowableInstrument> v;
        auto array = _impl->credit_get_borrowable_instruments(position_src,
                                                              strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }
        for (int i = 0; i < array->count(); i++) {
            auto in = array->at(i);
            OnBorrowableInstrument instrument;
            instrument.symbol = gbk_to_utf8(in.symbol);
            instrument.name = gbk_to_utf8(in.name);
            instrument.margin_rate_for_cash = in.margin_rate_for_cash;
            instrument.margin_rate_for_security = in.margin_rate_for_security;
            v.emplace_back(std::move(instrument));
        }
        array->release();
        return std::move(v);
    }

    Vec <OnBorrowableInstrumentPosition>
    strategy::credit_get_borrowable_instruments_positions(int position_src, String &account) const {
        Vec <OnBorrowableInstrumentPosition> v;
        auto array = _impl->credit_get_borrowable_instruments_positions(position_src,
                                                                        strlen(account.c_str()) == 0 ? nullptr
                                                                                                     : account.c_str());
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }
        for (int i = 0; i < array->count(); i++) {
            auto pos = array->at(i);
            OnBorrowableInstrumentPosition instrument;
            instrument.symbol = gbk_to_utf8(pos.symbol);
            instrument.name = gbk_to_utf8(pos.name);
            instrument.balance = pos.balance;
            instrument.available = pos.available;
            v.emplace_back(std::move(instrument));
        }
        array->release();
        return std::move(v);
    }

    rust::Vec<OnCreditContract>
    strategy::credit_get_contracts(int position_src, rust::String &account) const {
        rust::Vec<OnCreditContract> v;
        auto array = _impl->credit_get_contracts(position_src,
                                                 strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return v;
        }
        if (array->status() != 0) {
            return v;
        }
        for (int i = 0; i < array->count(); i++) {
            auto in = array->at(i);
            OnCreditContract c;
            c.symbol = gbk_to_utf8(in.symbol);
            c.name = gbk_to_utf8(in.name);
            c.orderdate = in.orderdate;
            c.ordersno = gbk_to_utf8(in.ordersno);
            c.creditdirect = in.creditdirect;
            c.orderqty = in.orderqty;
            c.matchqty = in.matchqty;
            c.orderamt = in.orderamt;
            c.orderfrzamt = in.orderfrzamt;
            c.matchamt = in.matchamt;
            c.clearamt = in.clearamt;
            c.lifestatus = in.lifestatus;
            c.enddate = in.enddate;
            c.oldenddate = in.oldenddate;
            c.creditrepay = in.creditrepay;
            c.creditrepayunfrz = in.creditrepayunfrz;
            c.fundremain = in.fundremain;
            c.stkrepay = in.stkrepay;
            c.stkrepayunfrz = in.stkrepayunfrz;
            c.stkremain = in.stkremain;
            c.stkremainvalue = in.stkremainvalue;
            c.fee = in.fee;
            c.overduefee = in.overduefee;
            c.fee_repay = in.fee_repay;
            c.punifee = in.punifee;
            c.punifee_repay = in.punifee_repay;
            c.rights = in.rights;
            c.overduerights = in.overduerights;
            c.rights_repay = in.rights_repay;
            c.lastprice = in.lastprice;
            c.profitcost = in.profitcost;
            c.sysdate = in.sysdate;
            c.sno = gbk_to_utf8(in.sno);
            c.lastdate = in.lastdate;
            c.closedate = in.closedate;
            c.punidebts = in.punidebts;
            c.punidebts_repay = in.punidebts_repay;
            c.punidebtsunfrz = in.punidebtsunfrz;
            c.punifeeunfrz = in.punifeeunfrz;
            c.punirights = in.punirights;
            c.punirights_repay = in.punirights_repay;
            c.punirightsunfrz = in.punirightsunfrz;
            c.feeunfrz = in.feeunfrz;
            c.rightsqty = in.rightsqty;
            c.overduerightsqty = in.overduerightsqty;
            v.emplace_back(std::move(c));
        }
        array->release();
        return std::move(v);
    }

    rust::Box<OnCreditCash> strategy::credit_get_cash(rust::String &account) const {
        CreditCash c;
        auto result = _impl->credit_get_cash(c, strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (result != 0) {
            return rust::Box<OnCreditCash>::from_raw(nullptr);
        }
        OnCreditCash *cash = new OnCreditCash;
        cash->fundintrrate = c.fundintrrate;
        cash->stkintrrate = c.stkintrrate;
        cash->punishintrrate = c.punishintrrate;
        cash->creditstatus = c.creditstatus;
        cash->marginrates = c.marginrates;
        cash->realrate = c.realrate;
        cash->asset = c.asset;
        cash->liability = c.liability;
        cash->marginavl = c.marginavl;
        cash->fundbal = c.fundbal;
        cash->fundavl = c.fundavl;
        cash->dsaleamtbal = c.dsaleamtbal;
        cash->guaranteeout = c.guaranteeout;
        cash->gagemktavl = c.gagemktavl;
        cash->fdealavl = c.fdealavl;
        cash->ffee = c.ffee;
        cash->ftotaldebts = c.ftotaldebts;
        cash->dealfmktavl = c.dealfmktavl;
        cash->dfee = c.dfee;
        cash->dtotaldebts = c.dtotaldebts;
        cash->fcreditbal = c.fcreditbal;
        cash->fcreditavl = c.fcreditavl;
        cash->fcreditfrz = c.fcreditfrz;
        cash->dcreditbal = c.dcreditbal;
        cash->dcreditavl = c.dcreditavl;
        cash->dcreditfrz = c.dcreditfrz;
        cash->rights = c.rights;
        cash->serviceuncomerqrights = c.serviceuncomerqrights;
        cash->rightsqty = c.rightsqty;
        cash->serviceuncomerqrightsqty = c.serviceuncomerqrightsqty;
        cash->acreditbal = c.acreditbal;
        cash->acreditavl = c.acreditavl;
        cash->acashcapital = c.acashcapital;
        cash->astkmktvalue = c.astkmktvalue;
        cash->withdrawable = c.withdrawable;
        cash->netcapital = c.netcapital;
        cash->fcreditpnl = c.fcreditpnl;
        cash->dcreditpnl = c.dcreditpnl;
        cash->fcreditmarginoccupied = c.fcreditmarginoccupied;
        cash->dcreditmarginoccupied = c.dcreditmarginoccupied;
        cash->collateralbuyableamt = c.collateralbuyableamt;
        cash->repayableamt = c.repayableamt;
        cash->dcreditcashavl = c.dcreditcashavl;
        return rust::Box<OnCreditCash>::from_raw(cash);
    }

    rust::Box<OnOrder>
    strategy::ipo_buy(rust::String &symbol, int volume, double price, rust::String &account) const {
        auto o = _impl->ipo_buy(symbol.c_str(), volume, price,
                                strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto order = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(order);
    }

    rust::Vec<OnIPOQI> strategy::ipo_get_quota(rust::String &account) const {
        rust::Vec<OnIPOQI> v;
        auto array = _impl->ipo_get_quota(strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return std::move(v);
        }
        if (array->status() != 0) {
            return std::move(v);
        }
        for (int i = 0; i < array->count(); i++) {
            auto in = array->at(i);
            OnIPOQI c;
            c.exchange = gbk_to_utf8(in.exchange);
            c.quota = in.quota;
            c.sse_star_quota = in.sse_star_quota;
            v.emplace_back(std::move(c));
        }

        array->release();
        return std::move(v);
    }

    rust::Vec<OnIPOInstruments> strategy::ipo_get_instruments(int security_type, String &account) const {
        rust::Vec<OnIPOInstruments> v;
        auto array = _impl->ipo_get_instruments(security_type,
                                                strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return std::move(v);
        }
        if (array->status() != 0) {
            return std::move(v);
        }
        for (int i = 0; i < array->count(); i++) {
            auto ipo = array->at(i);
            OnIPOInstruments c;
            c.symbol = gbk_to_utf8(ipo.symbol);
            c.price = ipo.price;
            c.min_vol = ipo.min_vol;
            c.max_vol = ipo.max_vol;
            v.emplace_back(std::move(c));
        }
        array->release();
        return std::move(v);
    }

    rust::Vec<OnIPOMatchNumber> strategy::ipo_get_match_number(String &account) const {
        rust::Vec<OnIPOMatchNumber> v;
        auto array = _impl->ipo_get_match_number(strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return std::move(v);
        }
        if (array->status() != 0) {
            return std::move(v);
        }
        for (int i = 0; i < array->count(); i++) {
            auto ipo = array->at(i);
            OnIPOMatchNumber c;
            c.order_id = gbk_to_utf8(ipo.order_id);
            c.symbol = gbk_to_utf8(ipo.symbol);
            c.volume = ipo.volume;
            c.match_number = gbk_to_utf8(ipo.match_number);
            c.order_at = ipo.order_at;
            c.match_at = ipo.match_at;
            v.emplace_back(std::move(c));
        }
        array->release();
        return std::move(v);
    }

    rust::Vec<OnIPOLotInfo> strategy::ipo_get_lot_info(String &account) const {
        rust::Vec<OnIPOLotInfo> v;
        auto array = _impl->ipo_get_lot_info(strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        if (array == nullptr) {
            return std::move(v);
        }
        if (array->status() != 0) {
            return std::move(v);
        }
        for (int i = 0; i < array->count(); i++) {
            auto ipo = array->at(i);
            OnIPOLotInfo c;
            c.symbol = gbk_to_utf8(ipo.symbol);
            c.order_at = ipo.order_at;
            c.lot_at = ipo.lot_at;
            c.lot_volume = ipo.lot_volume;
            c.give_up_volume = ipo.give_up_volume;
            c.price = ipo.price;
            c.amount = ipo.amount;
            c.pay_volume = ipo.pay_volume;
            c.pay_amount = ipo.pay_amount;
            v.emplace_back(std::move(c));
        }
        array->release();
        return std::move(v);
    }

    rust::Box<OnOrder> strategy::fund_etf_buy(String &symbol, int volume, double price, String &account) const {
        auto o = _impl->fund_etf_buy(symbol.c_str(), volume, price,
                                     strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto order = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(order);
    }

    rust::Box<OnOrder> strategy::fund_etf_redemption(String &symbol, int volume, double price, String &account) const {
        auto o = _impl->fund_etf_redemption(symbol.c_str(), volume, price,
                                            strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto order = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(order);
    }

    rust::Box<OnOrder> strategy::fund_subscribing(String &symbol, int volume, String &account) const {
        auto o = _impl->fund_subscribing(symbol.c_str(), volume,
                                         strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto order = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(order);
    }

    rust::Box<OnOrder> strategy::fund_buy(String &symbol, int volume, String &account) const {
        auto o = _impl->fund_buy(symbol.c_str(), volume,
                                 strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto order = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(order);
    }

    rust::Box<OnOrder> strategy::fund_redemption(String &symbol, int volume, String &account) const {
        auto o = _impl->fund_redemption(symbol.c_str(), volume,
                                        strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto order = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(order);
    }

    rust::Box<OnOrder>
    strategy::bond_reverse_repurchase_agreement(String &symbol, int volume, double price, int order_type,
                                                int order_duration, int order_qualifier, String &account) const {
        auto o = _impl->bond_reverse_repurchase_agreement(symbol.c_str(), volume, price, order_type, order_duration,
                                                          order_qualifier,
                                                          strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto order = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(order);
    }

    rust::Box<OnOrder>
    strategy::bond_convertible_call(String &symbol, int volume, double price, String &account) const {
        auto o = _impl->bond_convertible_call(symbol.c_str(), volume, price,
                                              strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto order = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(order);
    }

    rust::Box<OnOrder> strategy::bond_convertible_put(String &symbol, int volume, double price, String &account) const {
        auto o = _impl->bond_convertible_put(symbol.c_str(), volume, price,
                                             strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto order = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(order);
    }

    rust::Box<OnOrder> strategy::bond_convertible_put_cancel(String &symbol, int volume, String &account) const {
        auto o = _impl->bond_convertible_put_cancel(symbol.c_str(), volume,
                                                    strlen(account.c_str()) == 0 ? nullptr : account.c_str());
        auto order = order_to_on_order(o);
        return rust::Box<OnOrder>::from_raw(order);
    }

    int strategy::add_parameters(rust::Vec<rust::OnParameter> params) const {
        Parameter *p = new Parameter[params.size()];

        for (int i = 0; i < params.size(); i++) {
            strcpy(p[i].key, std::string(params[i].key).c_str());
            p[i].value = params[i].value;
            p[i].min = params[i].min;
            p[i].max = params[i].max;
            strcpy(p[i].name, std::string(params[i].name).c_str());
            strcpy(p[i].intro, std::string(params[i].intro).c_str());
            strcpy(p[i].group, std::string(params[i].group).c_str());
            p[i].readonly = params[i].readonly;
        }
        auto result = _impl->add_parameters(p, params.size());
        if (p != nullptr) {
            delete[] p;
        }
        return result;
    }

    int strategy::del_parameters(String &keys) const {
        return _impl->del_parameters(keys.c_str());
    }

    int strategy::set_parameters(rust::Vec<rust::OnParameter> params) const {
        Parameter *p = new Parameter[params.size()];
        for (int i = 0; i < params.size(); i++) {
            strcpy(p[i].key, std::string(params[i].key).c_str());
            p[i].value = params[i].value;
            p[i].min = params[i].min;
            p[i].max = params[i].max;
            strcpy(p[i].name, std::string(params[i].name).c_str());
            strcpy(p[i].intro, std::string(params[i].intro).c_str());
            strcpy(p[i].group, std::string(params[i].group).c_str());
            p[i].readonly = params[i].readonly;
        }
        auto result = _impl->set_parameters(p, params.size());
        if (p != nullptr) {
            delete[] p;
        }
        return result;
    }

    rust::Vec<OnParameter> strategy::get_parameters() const {
        rust::Vec<OnParameter> v;
        auto array = _impl->get_parameters();
        if (array == nullptr) {
            return v;
        }

        if (array->status() != 0) {
            return v;
        }

        for (int i = 0; i < array->count(); i++) {
            auto p = array->at(i);
            OnParameter param;
            param.key = gbk_to_utf8(p.key);
            param.value = p.value;
            param.min = p.min;
            param.max = p.max;
            param.name = gbk_to_utf8(p.name);
            param.intro = gbk_to_utf8(p.intro);
            param.group = gbk_to_utf8(p.group);
            param.readonly = p.readonly;
            v.push_back(param);
        }
        array->release();
        return std::move(v);
    }

    int strategy::set_symbols(String &symbols) const {
        return _impl->set_symbols(symbols.c_str());
    }

    rust::Vec<rust::String> strategy::get_symbols() const {
        rust::Vec<rust::String> v;
        auto array = _impl->get_symbols();
        if (array == nullptr) {
            return v;
        }

        if (array->status() != 0) {
            return v;
        }

        for (int i = 0; i < array->count(); i++) {
            auto s = array->at(i);
            v.push_back(gbk_to_utf8(s));
        }
        array->release();
        return std::move(v);
    }

    void strategy::set_callback_instantiate(rust::CallbackImpl *callback) {
        this->_callback = callback;
    }

    rust::Box<CallbackImpl> strategy::get_callback_instantiate() const {
        rust::CallbackImpl *callback = this->_callback;
        return rust::Box<rust::CallbackImpl>::from_raw(callback);
    }

    std::shared_ptr <strategy>
    instantiate_strategy() {
        auto call = rust::new_callback();
        auto ptr = call.into_raw();
        auto ins = std::make_shared<strategy>(ptr);
        ins->set_callback_instantiate(ptr);
        return ins;
    }
}