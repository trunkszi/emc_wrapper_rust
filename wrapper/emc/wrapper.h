//
// Created by Administrator on 2022/8/7.
//
#pragma once

#include "rust/cxx.h"

#include <memory>

namespace rust
{
    struct OnTick;
    struct OnBar;
    struct OnL2Transaction;
    struct OnL2Order;
    struct OnOrder;
    struct OnExecRpt;
    struct OnAlgoOrder;
    struct OnCash;
    struct OnPosition;
    struct OnParameter;
    struct OnIndicator;
    struct OnAccountStatus;
    struct OnL2OrderQueue;
    struct OnIPOMatchNumber;
    struct OnIPOLotInfo;
    struct OnCreditCash;
    struct OnBorrowableInstrumentPosition;
    struct OnCollateralInstrument;
    struct OnAccount;
    struct OnCreditCash;
    struct RepayCashParam;
    struct OnBorrowableInstrument;
    struct OnCreditCash;
    struct OnCreditContract;
    struct OnIPOQI;
    struct OnIPOInstruments;
    struct CallbackImpl;

    class strategy : public std::enable_shared_from_this<strategy>
    {
    public:
         strategy(rust::CallbackImpl *callback);
//        strategy(rust::CallbackImpl &_callback);

        ~strategy();

        std::shared_ptr<strategy> ptr();

        int run() const;

        void set_mode(int mode) const;

        void stop() const;

        double now() const;

        void set_strategy_id(rust::String &strategy_id) const;

        void set_token(rust::String &token) const;

        int schedule(rust::String &data_rule, rust::String &time_rule) const;

        int set_backtest_config(
            rust::String &start_time,
            rust::String &end_time,
            double initial_cash,
            double transaction_ratio,
            double commission_ratio,
            double slippage_ratio,
            int adjust,
            int check_cache) const;

        int subscribe(rust::String &symbols, rust::String &frequency, bool unsubscribe_previous) const;

        int unsubscribe(rust::String &symbols, rust::String &frequency) const;

        rust::Vec<rust::OnAccount> get_accounts() const;

        rust::Box<rust::OnAccountStatus> get_account_status(rust::String &account) const;

        rust::Vec<rust::OnAccountStatus> get_all_account_status() const;

        rust::Box<rust::OnOrder>
        order_volume(rust::String &symbol, int volume, int side, int order_type, int position_effect, double price,
                     rust::String &account) const;

        rust::Box<rust::OnOrder>
        order_value(rust::String &symbol, double value, int side, int order_type, int position_effect, double price,
                    rust::String &account) const;

        rust::Box<rust::OnOrder>
        order_percent(rust::String &symbol, double percent, int side, int order_type, int position_effect, double price,
                      rust::String &account) const;

        rust::Box<rust::OnOrder>
        order_target_volume(rust::String &symbol, int volume, int position_side, int order_type, double price,
                            rust::String &account) const;

        rust::Box<rust::OnOrder>
        order_target_value(rust::String &symbol, double value, int position_side, int order_type, double price,
                           rust::String &account) const;

        rust::Box<rust::OnOrder>
        order_target_percent(rust::String &symbol, double percent, int position_side, int order_type, double price,
                             rust::String &account) const;

        rust::Vec<rust::OnOrder> order_close_all() const;

        int order_cancel(rust::String &cl_ord_id, rust::String &account) const;

        int order_cancel_all() const;

        rust::Box<rust::OnOrder>
        place_order(rust::String &symbol, int volume, int side, int order_type, int position_effect, double price,
                    int order_duration, int order_qualifier, double stop_price, int order_business,
                    rust::String &account) const;

        rust::Box<rust::OnOrder>
        order_after_hour(rust::String &symbol, int volume, int side, double price, rust::String &account) const;

        rust::Vec<rust::OnOrder> get_orders(rust::String &account) const;

        rust::Vec<rust::OnOrder> get_unfinished_orders(rust::String &account) const;

        rust::Vec<rust::OnExecRpt> get_execution_reports(rust::String &account) const;

        rust::Vec<rust::OnCash> get_cash(rust::String &account) const;

        rust::Vec<rust::OnPosition> get_position(rust::String &account) const;

        rust::Box<rust::OnAlgoOrder>
        order_algo(rust::String &symbol, int volume, int position_effect, int side, int order_type, double price,
                   rust::String &algo_name, rust::String &algo_param, rust::String &account) const;

        int algo_order_cancel(rust::String &cl_ord_id, rust::String &account) const;

        rust::Vec<rust::OnOrder> get_algo_child_orders(rust::String &cl_ord_id, rust::String &account) const;

        rust::String raw_func(rust::String &account, rust::String &func_id, rust::String &func_args) const;

        rust::Box<rust::OnOrder>
        credit_buying_on_margin(int position_src, rust::String &symbol, int volume, double price,
                                int order_type,
                                int order_duration,
                                int order_qualifier, rust::String &account) const;

        rust::Box<rust::OnOrder> credit_short_selling(int position_src, rust::String &symbol, int volume, double price,
                                                      int order_type, int order_duration,
                                                      int order_qualifier, rust::String &account) const;

        rust::Box<rust::OnOrder> credit_repay_share_by_buying_share(rust::String &symbol, int volume, double price,
                                                                    int order_type,
                                                                    int order_duration,
                                                                    int order_qualifier,
                                                                    rust::String &account) const;

        rust::Box<rust::OnOrder> credit_repay_cash_by_selling_share(rust::String &symbol, int volume, double price,
                                                                    int order_type,
                                                                    int order_duration,
                                                                    int order_qualifier,
                                                                    rust::String &account) const;

        rust::Box<rust::OnOrder>
        credit_buying_on_collateral(rust::String &symbol, int volume, double price, int order_type,
                                    int order_duration,
                                    int order_qualifier, rust::String &account) const;

        rust::Box<rust::OnOrder>
        credit_selling_on_collateral(rust::String &symbol, int volume, double price, int order_type,
                                     int order_duration,
                                     int order_qualifier, rust::String &account) const;

        rust::Box<rust::OnOrder>
        credit_repay_share_directly(rust::String &symbol, int volume, rust::String &account) const;

        rust::Box<rust::RepayCashParam> credit_repay_cash_directly(double amount, rust::String &account) const;

        rust::Box<rust::OnOrder> credit_collateral_in(rust::String &symbol, int volume, rust::String &account) const;

        rust::Box<rust::OnOrder> credit_collateral_out(rust::String &symbol, int volume, rust::String &account) const;

        rust::Vec<rust::OnCollateralInstrument>
        credit_get_collateral_instruments(rust::String &account) const;

        rust::Vec<rust::OnBorrowableInstrument>
        credit_get_borrowable_instruments(int position_src, rust::String &account) const;

        rust::Vec<rust::OnBorrowableInstrumentPosition>
        credit_get_borrowable_instruments_positions(int position_src, rust::String &account) const;

        rust::Vec<rust::OnCreditContract> credit_get_contracts(int position_src, rust::String &account) const;

        rust::Box<rust::OnCreditCash> credit_get_cash(rust::String &account) const;

        rust::Box<rust::OnOrder>
        ipo_buy(rust::String &symbol, int volume, double price, rust::String &account) const;

        rust::Vec<rust::OnIPOQI> ipo_get_quota(rust::String &account) const;

        rust::Vec<rust::OnIPOInstruments> ipo_get_instruments(int security_type, rust::String &account) const;

        rust::Vec<rust::OnIPOMatchNumber> ipo_get_match_number(rust::String &account) const;

        rust::Vec<rust::OnIPOLotInfo> ipo_get_lot_info(rust::String &account) const;

        rust::Box<rust::OnOrder>
        fund_etf_buy(rust::String &symbol, int volume, double price, rust::String &account) const;

        rust::Box<rust::OnOrder>
        fund_etf_redemption(rust::String &symbol, int volume, double price, rust::String &account) const;

        rust::Box<rust::OnOrder> fund_subscribing(rust::String &symbol, int volume, rust::String &account) const;

        rust::Box<rust::OnOrder> fund_buy(rust::String &symbol, int volume, rust::String &account) const;

        rust::Box<rust::OnOrder> fund_redemption(rust::String &symbol, int volume, rust::String &account) const;

        rust::Box<rust::OnOrder> bond_reverse_repurchase_agreement(rust::String &symbol, int volume, double price,
                                                                   int order_type,
                                                                   int order_duration,
                                                                   int order_qualifier,
                                                                   rust::String &account) const;

        rust::Box<rust::OnOrder>
        bond_convertible_call(rust::String &symbol, int volume, double price, rust::String &account) const;

        rust::Box<rust::OnOrder>
        bond_convertible_put(rust::String &symbol, int volume, double price, rust::String &account) const;

        rust::Box<rust::OnOrder>
        bond_convertible_put_cancel(rust::String &symbol, int volume, rust::String &account) const;

        int add_parameters(rust::Vec<rust::OnParameter> params) const;

        int del_parameters(rust::String &keys) const;

        int set_parameters(rust::Vec<rust::OnParameter> params) const;

        rust::Vec<rust::OnParameter> get_parameters() const;

        int set_symbols(rust::String &symbols) const;

        rust::Vec<rust::String> get_symbols() const;

        rust::Box<CallbackImpl> get_callback_instantiate() const;

        void set_callback_instantiate(rust::CallbackImpl *_callback) ;

    private:
        class strategy::impl;
        friend class strategy::impl;
        strategy::impl *_impl = nullptr;
        rust::CallbackImpl *_callback = nullptr;
    };

    std::shared_ptr<strategy> instantiate_strategy();
}
