#![allow(unused, unused_variables)]

use std::borrow::BorrowMut;
use std::cell::RefCell;
use std::marker::PhantomPinned;
use std::marker::Sized;
use std::os::raw::c_char;
use std::pin::Pin;
use std::ptr::NonNull;
use std::rc::Rc;
use std::rc::Weak;

pub mod error;
pub mod tests;

#[cxx::bridge(namespace = "rust")]
pub mod ffi {
    #[derive(Debug)]
    pub struct RepayCashParam {
        pub actual_repay_amount: f64,
        pub error_msg_buf: String,
        pub error_code: i32,
    }

    #[derive(Debug)]
    pub struct OnBar {
        pub symbol: String,
        pub bob: f64,
        pub eob: f64,
        pub open: f32,
        pub close: f32,
        pub high: f32,
        pub low: f32,
        pub volume: f64,
        pub amount: f64,
        pub pre_close: f32,
        pub position: i64,
        pub frequency: String,
    }

    #[derive(Debug)]
    pub struct OnQuote {
        pub bid_price: f32,
        pub bid_volume: i64,
        pub ask_price: f32,
        pub ask_volume: i64,
    }

    #[derive(Debug)]
    pub struct OnTick {
        pub symbol: String,
        pub created_at: f64,
        pub price: f32,
        pub open: f32,
        pub high: f32,
        pub low: f32,
        pub cum_volume: f64,
        pub cum_amount: f64,
        pub cum_position: i64,
        pub last_amount: f64,
        pub last_volume: i32,
        pub trade_type: i32,
        pub quotes: Vec<OnQuote>,
    }

    #[derive(Debug)]
    pub struct OnL2Transaction {
        pub symbol: String,
        pub created_at: f64,
        pub price: f32,
        pub volume: i64,
        pub side: i8,
        pub exec_type: i8,
        pub exec_index: i64,
        pub ask_order_index: i64,
        pub bid_order_index: i64,
    }

    #[derive(Debug)]
    pub struct OnL2Order {
        pub symbol: String,
        pub created_at: f64,
        pub price: f32,
        pub volume: i64,
        pub side: i8,
        pub order_type: i8,
        pub order_index: i64,
    }

    #[derive(Debug)]
    pub struct OnL2OrderQueue {
        pub symbol: String,
        pub created_at: f64,
        pub price: f32,
        pub volume: i64,
        pub side: i8,
        pub queue_orders: i32,
        pub queue_volumes: Vec<i32>,
    }

    #[derive(Debug)]
    pub struct OnOrder {
        pub strategy_id: String,
        pub account_id: String,
        pub account_name: String,
        pub cl_ord_id: String,
        pub order_id: String,
        pub ex_ord_id: String,
        pub algo_order_id: String,
        pub order_business: i32,

        pub symbol: String,
        pub side: i32,
        pub position_effect: i32,
        pub position_side: i32,

        pub order_type: i32,
        pub order_duration: i32,
        pub order_qualifier: i32,
        pub order_src: i32,
        pub position_src: i32,

        pub status: i32,
        pub ord_rej_reason: i32,
        pub ord_rej_reason_detail: String,

        pub price: f64,
        pub stop_price: f64,

        pub order_style: i32,
        pub volume: i64,
        pub value: f64,
        pub percent: f64,
        pub target_volume: i64,
        pub target_value: f64,
        pub target_percent: f64,
        pub filled_volume: i64,
        pub filled_vwap: f64,
        pub filled_amount: f64,
        pub filled_commission: f64,

        pub created_at: i64,
        pub updated_at: i64,
    }

    #[derive(Debug)]
    pub struct OnAlgoOrder {
        pub strategy_id: String,
        pub account_id: String,
        pub account_name: String,

        pub cl_ord_id: String,
        pub order_id: String,
        pub ex_ord_id: String,
        pub order_business: i32,
        pub symbol: String,
        pub side: i32,
        pub position_effect: i32,
        pub position_side: i32,

        pub order_type: i32,
        pub order_duration: i32,
        pub order_qualifier: i32,
        pub order_src: i32,
        pub position_src: i32,

        pub status: i32,
        pub ord_rej_reason: i32,
        pub ord_rej_reason_detail: String,

        pub price: f64,
        pub stop_price: f64,

        pub order_style: i32,
        pub volume: i64,
        pub value: f64,
        pub percent: f64,
        pub target_volume: i64,
        pub target_value: f64,
        pub target_percent: f64,

        pub filled_volume: i64,
        pub filled_vwap: f64,
        pub filled_amount: f64,
        pub filled_commission: f64,

        pub algo_name: String,
        pub algo_param: String,
        pub algo_status: i32,
        pub algo_comment: String,

        pub created_at: i64,
        pub updated_at: i64,
    }

    #[derive(Debug)]
    pub struct OnExecRpt {
        pub strategy_id: String,
        pub account_id: String,
        pub account_name: String,

        pub cl_ord_id: String,
        pub order_id: String,
        pub exec_id: String,

        pub symbol: String,

        pub position_effect: i32,
        pub side: i32,
        pub ord_rej_reason: i32,
        pub ord_rej_reason_detail: String,
        pub exec_type: i32,

        pub price: f64,
        pub volume: i64,
        pub amount: f64,
        pub commission: f64,
        pub cost: f64,
        pub created_at: i64,
    }

    #[derive(Debug)]
    pub struct OnCash {
        pub account_id: String,
        pub account_name: String,

        pub currency: i32,

        pub nav: f64,
        pub pnl: f64,
        pub fpnl: f64,
        pub frozen: f64,
        pub order_frozen: f64,
        pub available: f64,
        pub balance: f64,
        pub market_value: f64,
        pub cum_inout: f64,
        pub cum_trade: f64,
        pub cum_pnl: f64,
        pub cum_commission: f64,

        pub last_trade: f64,
        pub last_pnl: f64,
        pub last_commission: f64,
        pub last_inout: f64,
        pub change_reason: i32,
        pub change_event_id: String,

        pub created_at: i64,
        pub updated_at: i64,
    }

    #[derive(Debug)]
    pub struct OnPosition {
        pub account_id: String,
        pub account_name: String,

        pub symbol: String,
        pub side: i32,
        pub volume: i64,
        pub volume_today: i64,
        pub vwap: f64,
        pub vwap_diluted: f64,
        pub vwap_open: f64,
        pub amount: f64,

        pub price: f64,
        pub fpnl: f64,
        pub fpnl_open: f64,
        pub cost: f64,
        pub order_frozen: i64,
        pub order_frozen_today: i64,
        pub available: i64,
        pub available_today: i64,
        pub available_now: i64,
        pub market_value: f64,

        pub last_price: f64,
        pub last_volume: i64,
        pub last_inout: i64,
        pub change_reason: i32,
        pub change_event_id: String,

        pub has_dividend: i32,
        pub created_at: i64,
        pub updated_at: i64,
    }

    #[derive(Debug)]
    pub struct OnAccount {
        pub account_id: String,
        pub account_name: String,
        pub title: String,
        pub intro: String,
        pub comment: String,
    }

    #[derive(Debug)]
    pub struct OnAccountStatus {
        pub account_id: String,
        pub account_name: String,
        pub state: i32,
        pub error_code: i32,
        pub error_msg: String,
    }

    #[derive(Debug)]
    pub struct OnParameter {
        pub key: String,
        pub value: f64,
        pub min: f64,
        pub max: f64,
        pub name: String,
        pub intro: String,
        pub group: String,
        pub readonly: bool,
    }

    #[derive(Debug)]
    pub struct OnIndicator {
        pub account_id: String,
        pub pnl_ratio: f64,
        pub pnl_ratio_annual: f64,
        pub sharp_ratio: f64,
        pub max_drawdown: f64,
        pub risk_ratio: f64,
        pub open_count: i32,
        pub close_count: i32,
        pub win_count: i32,
        pub lose_count: i32,
        pub win_ratio: f64,

        pub created_at: i64,
        pub updated_at: i64,
    }

    #[derive(Debug)]
    pub struct OnCollateralInstrument {
        pub symbol: String,
        pub name: String,
        pub pledge_rate: f64,
    }

    #[derive(Debug)]
    pub struct OnBorrowableInstrument {
        pub symbol: String,
        pub name: String,
        pub margin_rate_for_cash: f64,
        pub margin_rate_for_security: f64,
    }

    #[derive(Debug)]
    pub struct OnBorrowableInstrumentPosition {
        pub symbol: String,
        pub name: String,
        pub balance: f64,
        pub available: f64,
    }

    #[derive(Debug)]
    pub struct OnCreditContract {
        pub symbol: String,
        pub name: String,
        pub orderdate: i32,
        pub ordersno: String,
        pub creditdirect: i8,
        pub orderqty: f64,
        pub matchqty: f64,
        pub orderamt: f64,
        pub orderfrzamt: f64,
        pub matchamt: f64,
        pub clearamt: f64,
        pub lifestatus: i8,
        pub enddate: i32,
        pub oldenddate: i32,
        pub creditrepay: f64,
        pub creditrepayunfrz: f64,
        pub fundremain: f64,
        pub stkrepay: f64,
        pub stkrepayunfrz: f64,
        pub stkremain: f64,
        pub stkremainvalue: f64,
        pub fee: f64,
        pub overduefee: f64,
        pub fee_repay: f64,
        pub punifee: f64,
        pub punifee_repay: f64,
        pub rights: f64,
        pub overduerights: f64,
        pub rights_repay: f64,
        pub lastprice: f64,
        pub profitcost: f64,
        pub sysdate: i32,
        pub sno: String,
        pub lastdate: i32,
        pub closedate: i32,
        pub punidebts: f64,
        pub punidebts_repay: f64,
        pub punidebtsunfrz: f64,
        pub punifeeunfrz: f64,
        pub punirights: f64,
        pub punirights_repay: f64,
        pub punirightsunfrz: f64,
        pub feeunfrz: f64,
        pub overduefeeunfrz: f64,
        pub rightsqty: f64,
        pub overduerightsqty: f64,
    }

    #[derive(Debug)]
    pub struct OnCreditCash {
        pub fundintrrate: f64,
        pub stkintrrate: f64,
        pub punishintrrate: f64,
        pub creditstatus: i8,
        pub marginrates: f64,
        pub realrate: f64,
        pub asset: f64,
        pub liability: f64,
        pub marginavl: f64,
        pub fundbal: f64,
        pub fundavl: f64,
        pub dsaleamtbal: f64,
        pub guaranteeout: f64,
        pub gagemktavl: f64,
        pub fdealavl: f64,
        pub ffee: f64,
        pub ftotaldebts: f64,
        pub dealfmktavl: f64,
        pub dfee: f64,
        pub dtotaldebts: f64,
        pub fcreditbal: f64,
        pub fcreditavl: f64,
        pub fcreditfrz: f64,
        pub dcreditbal: f64,
        pub dcreditavl: f64,
        pub dcreditfrz: f64,
        pub rights: f64,
        pub serviceuncomerqrights: f64,
        pub rightsqty: f64,
        pub serviceuncomerqrightsqty: f64,
        pub acreditbal: f64,
        pub acreditavl: f64,
        pub acashcapital: f64,
        pub astkmktvalue: f64,
        pub withdrawable: f64,
        pub netcapital: f64,
        pub fcreditpnl: f64,
        pub dcreditpnl: f64,
        pub fcreditmarginoccupied: f64,
        pub dcreditmarginoccupied: f64,
        pub collateralbuyableamt: f64,
        pub repayableamt: f64,
        pub dcreditcashavl: f64,
    }

    #[derive(Debug)]
    pub struct OnIPOQI {
        pub exchange: String,
        pub quota: f64,
        pub sse_star_quota: f64,
    }

    #[derive(Debug)]
    pub struct OnIPOInstruments {
        pub symbol: String,
        pub price: f64,
        pub min_vol: i32,
        pub max_vol: i32,
    }

    #[derive(Debug)]
    pub struct OnIPOMatchNumber {
        pub order_id: String,
        pub symbol: String,
        pub volume: i32,
        pub match_number: String,
        pub order_at: i32,
        pub match_at: i32,
    }

    #[derive(Debug)]
    pub struct OnIPOLotInfo {
        pub symbol: String,
        pub order_at: i32,
        pub lot_at: i32,
        pub lot_volume: i32,
        pub give_up_volume: i32,
        pub price: f64,
        pub amount: f64,
        pub pay_volume: f64,
        pub pay_amount: f64,
    }

    // C++ types and signatures exposed to Rust.

    unsafe extern "C++" {
        include!("wrapper.h");
        pub type strategy;

        pub fn run(self: &strategy) -> i32;
        pub fn stop(self: &strategy);
        pub fn now(self: &strategy) -> f64;
        pub fn set_strategy_id(self: &strategy, strategy_id: &mut String);
        pub fn set_token(self: &strategy, token: &mut String);
        pub fn schedule(self: &strategy, data_rule: &mut String, time_rule: &mut String) -> i32;
        pub fn set_backtest_config(
            self: &strategy,
            start_time: &mut String,
            end_time: &mut String,
            initial_cash: f64,
            transaction_ratio: f64,
            commission_ratio: f64,
            slippage_ratio: f64,
            adjust: i32,
            check_cache: i32,
        ) -> i32;
        pub fn subscribe(
            self: &strategy,
            symbols: &mut String,
            frequency: &mut String,
            unsubscribe_previous: bool,
        ) -> i32;
        pub fn unsubscribe(self: &strategy, symbols: &mut String, frequency: &mut String) -> i32;

        pub fn get_accounts(self: &strategy) -> Vec<OnAccount>;
        pub fn get_account_status(self: &strategy, account: &mut String) -> Box<OnAccountStatus>;

        pub fn get_all_account_status(self: &strategy) -> Vec<OnAccountStatus>;

        pub fn order_volume(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            side: i32,
            order_type: i32,
            position_effect: i32,
            price: f64,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn order_value(
            self: &strategy,
            symbol: &mut String,
            value: f64,
            side: i32,
            order_type: i32,
            position_effect: i32,
            price: f64,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn order_percent(
            self: &strategy,
            symbol: &mut String,
            percent: f64,
            side: i32,
            order_type: i32,
            position_effect: i32,
            price: f64,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn order_target_volume(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            side: i32,
            order_type: i32,
            price: f64,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn order_target_value(
            self: &strategy,
            symbol: &mut String,
            value: f64,
            side: i32,
            order_type: i32,
            price: f64,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn order_target_percent(
            self: &strategy,
            symbol: &mut String,
            percent: f64,
            side: i32,
            order_type: i32,
            price: f64,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn order_close_all(self: &strategy) -> Vec<OnOrder>;

        pub fn order_cancel(self: &strategy, cl_ord_id: &mut String, account: &mut String) -> i32;

        pub fn order_cancel_all(self: &strategy) -> i32;

        pub fn place_order(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            side: i32,
            order_type: i32,
            position_effect: i32,
            price: f64,
            order_duration: i32,
            order_qualifier: i32,
            stop_price: f64,
            order_business: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn order_after_hour(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            side: i32,
            price: f64,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn get_orders(self: &strategy, account: &mut String) -> Vec<OnOrder>;

        pub fn get_unfinished_orders(self: &strategy, account: &mut String) -> Vec<OnOrder>;

        pub fn get_execution_reports(self: &strategy, account: &mut String) -> Vec<OnExecRpt>;

        pub fn get_cash(self: &strategy, account: &mut String) -> Vec<OnCash>;

        pub fn get_position(self: &strategy, account: &mut String) -> Vec<OnPosition>;

        pub fn order_algo(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            position_effect: i32,
            side: i32,
            order_type: i32,
            price: f64,
            algo_name: &mut String,
            algo_param: &mut String,
            account: &mut String,
        ) -> Box<OnAlgoOrder>;

        pub fn algo_order_cancel(
            self: &strategy,
            cl_ord_id: &mut String,
            account: &mut String,
        ) -> i32;

        pub fn get_algo_child_orders(
            self: &strategy,
            cl_ord_id: &mut String,
            account: &mut String,
        ) -> Vec<OnOrder>;

        pub fn raw_func(
            self: &strategy,
            account: &mut String,
            func_id: &mut String,
            func_args: &mut String,
        ) -> String;

        pub fn credit_buying_on_margin(
            self: &strategy,
            position_src: i32,
            symbol: &mut String,
            volume: i32,
            price: f64,
            order_type: i32,
            order_duration: i32,
            order_qualifier: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn credit_short_selling(
            self: &strategy,
            position_src: i32,
            symbol: &mut String,
            volume: i32,
            price: f64,
            order_type: i32,
            order_duration: i32,
            order_qualifier: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn credit_repay_share_by_buying_share(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            price: f64,
            order_type: i32,
            order_duration: i32,
            order_qualifier: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn credit_repay_cash_by_selling_share(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            price: f64,
            order_type: i32,
            order_duration: i32,
            order_qualifier: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn credit_buying_on_collateral(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            price: f64,
            order_type: i32,
            order_duration: i32,
            order_qualifier: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn credit_selling_on_collateral(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            price: f64,
            order_type: i32,
            order_duration: i32,
            order_qualifier: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn credit_repay_share_directly(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn credit_repay_cash_directly(
            self: &strategy,
            amount: f64,
            account: &mut String,
        ) -> Box<RepayCashParam>;

        pub fn credit_collateral_in(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn credit_collateral_out(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn credit_get_collateral_instruments(
            self: &strategy,
            account: &mut String,
        ) -> Vec<OnCollateralInstrument>;

        pub fn credit_get_borrowable_instruments(
            self: &strategy,
            position_src: i32,
            account: &mut String,
        ) -> Vec<OnBorrowableInstrument>;

        pub fn credit_get_borrowable_instruments_positions(
            self: &strategy,
            position_src: i32,
            account: &mut String,
        ) -> Vec<OnBorrowableInstrumentPosition>;

        pub fn credit_get_contracts(
            self: &strategy,
            position_src: i32,
            account: &mut String,
        ) -> Vec<OnCreditContract>;

        pub fn credit_get_cash(self: &strategy, account: &mut String) -> Box<OnCreditCash>;

        pub fn ipo_buy(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            price: f64,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn ipo_get_quota(self: &strategy, account: &mut String) -> Vec<OnIPOQI>;

        pub fn ipo_get_instruments(
            self: &strategy,
            security_type: i32,
            account: &mut String,
        ) -> Vec<OnIPOInstruments>;

        pub fn ipo_get_match_number(self: &strategy, account: &mut String)
                                    -> Vec<OnIPOMatchNumber>;

        pub fn ipo_get_lot_info(self: &strategy, account: &mut String) -> Vec<OnIPOLotInfo>;

        pub fn fund_etf_buy(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            price: f64,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn fund_etf_redemption(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            price: f64,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn fund_subscribing(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn fund_buy(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn fund_redemption(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn bond_reverse_repurchase_agreement(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            price: f64,
            order_type: i32,
            order_duration: i32,
            order_qualifier: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn bond_convertible_call(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            price: f64,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn bond_convertible_put(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            price: f64,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn bond_convertible_put_cancel(
            self: &strategy,
            symbol: &mut String,
            volume: i32,
            account: &mut String,
        ) -> Box<OnOrder>;

        pub fn add_parameters(self: &strategy, params: Vec<OnParameter>) -> i32;

        pub fn del_parameters(self: &strategy, keys: &mut String) -> i32;

        pub fn set_parameters(self: &strategy, params: Vec<OnParameter>) -> i32;

        pub fn get_parameters(self: &strategy) -> Vec<OnParameter>;

        pub fn set_symbols(self: &strategy, symbols: &mut String) -> i32;

        pub fn get_symbols(self: &strategy) -> Vec<String>;

        pub fn set_mode(self: &strategy, mode: i32);

        pub fn get_callback_instantiate(self: &strategy) -> Box<CallbackImpl>;

        pub fn instantiate_strategy() -> SharedPtr<strategy>;
    }

    extern "Rust" {
        type CallbackImpl;
        fn on_init(self: &CallbackImpl);
        fn on_tick(self: &CallbackImpl, tick: Box<OnTick>);
        fn on_bar(self: &CallbackImpl, bar: Box<OnBar>);
        fn on_l2transaction(self: &CallbackImpl, transaction: Box<OnL2Transaction>);
        fn on_l2order(self: &CallbackImpl, order: Box<OnL2Order>);
        fn on_l2order_queue(self: &CallbackImpl, order_queue: Box<OnL2OrderQueue>);
        fn on_order_status(self: &CallbackImpl, order: Box<OnOrder>);
        fn on_execution_report(self: &CallbackImpl, rpt: Box<OnExecRpt>);
        fn on_algo_order_status(self: &CallbackImpl, order: Box<OnAlgoOrder>);
        fn on_cash(self: &CallbackImpl, cash: Box<OnCash>);
        fn on_position(self: &CallbackImpl, pos: Box<OnPosition>);
        fn on_parameter(self: &CallbackImpl, param: Box<OnParameter>);
        fn on_schedule(self: &CallbackImpl, data_rule: String, time_rule: String);
        fn on_backtest_finished(self: &CallbackImpl, indicator: Box<OnIndicator>);
        fn on_account_status(self: &CallbackImpl, status: Box<OnAccountStatus>);
        fn on_error(self: &CallbackImpl, error_code: String, error_msg: String);
        fn on_stop(self: &CallbackImpl);
        fn on_market_data_connected(self: &CallbackImpl);
        fn on_trade_data_connected(self: &CallbackImpl);
        fn on_market_data_disconnected(self: &CallbackImpl);
        fn on_trade_data_disconnected(self: &CallbackImpl);
        fn new_callback() -> Box<CallbackImpl>;
    }
}

pub type Tick = ffi::OnTick;
pub type Bar = ffi::OnBar;
pub type L2Transaction = ffi::OnL2Transaction;
pub type L2Order = ffi::OnL2Order;
pub type Order = ffi::OnOrder;
pub type ExecRpt = ffi::OnExecRpt;
pub type AlgoOrder = ffi::OnAlgoOrder;
pub type Cash = ffi::OnCash;
pub type Position = ffi::OnPosition;
pub type Parameter = ffi::OnParameter;
pub type Indicator = ffi::OnIndicator;
pub type AccountStatus = ffi::OnAccountStatus;
pub type L2OrderQueue = ffi::OnL2OrderQueue;
pub type IPOMatchNumber = ffi::OnIPOMatchNumber;
pub type IPOLotInfo = ffi::OnIPOLotInfo;
pub type IPOQI = ffi::OnIPOQI;
pub type CreditCash = ffi::OnCreditCash;
pub type CreditContract = ffi::OnCreditContract;
pub type BorrowableInstrumentPosition = ffi::OnBorrowableInstrumentPosition;
pub type BorrowableInstrument = ffi::OnBorrowableInstrument;
pub type CollateralInstrument = ffi::OnCollateralInstrument;
pub type IPOInstruments = ffi::OnIPOInstruments;
pub type Account = ffi::OnAccount;
pub type RepayCashParam = ffi::RepayCashParam;

pub trait CallbackTrait {
    fn on_init(&self);
    fn on_tick(&self, tick: Box<Tick>);
    fn on_bar(&self, bar: Box<Bar>);
    fn on_l2transaction(&self, transaction: Box<L2Transaction>);
    fn on_l2order(&self, order: Box<L2Order>);
    fn on_l2order_queue(&self, order_queue: Box<L2OrderQueue>);
    fn on_order_status(&self, order: Box<Order>);
    fn on_execution_report(&self, rpt: Box<ExecRpt>);
    fn on_algo_order_status(&self, order: Box<AlgoOrder>);
    fn on_cash(&self, cash: Box<Cash>);
    fn on_position(&self, pos: Box<Position>);
    fn on_parameter(&self, param: Box<Parameter>);
    fn on_schedule(&self, data_rule: String, time_rule: String);
    fn on_backtest_finished(&self, indicator: Box<Indicator>);
    fn on_account_status(&self, status: Box<AccountStatus>);
    fn on_error(&self, error_code: String, error_msg: String);
    fn on_stop(&self);
    fn on_market_data_connected(&self);
    fn on_trade_data_connected(&self);
    fn on_market_data_disconnected(&self);
    fn on_trade_data_disconnected(&self);
}

// struct DefaultImpl {}

// impl CallbackTrait for DefaultImpl {
//     fn on_init(&self) {}
//     fn on_tick(&self, tick: Box<Tick>) {}
//     fn on_bar(&self, bar: Box<Bar>) {}
//     fn on_l2transaction(&self, transaction: Box<L2Transaction>) {}
//     fn on_l2order(&self, order: Box<L2Order>) {}
//     fn on_l2order_queue(&self, order_queue: Box<L2OrderQueue>) {}
//     fn on_order_status(&self, order: Box<Order>) {}
//     fn on_execution_report(&self, rpt: Box<ExecRpt>) {}
//     fn on_algo_order_status(&self, order: Box<AlgoOrder>) {}
//     fn on_cash(&self, cash: Box<Cash>) {}
//     fn on_position(&self, pos: Box<Position>) {}
//     fn on_parameter(&self, param: Box<Parameter>) {}
//     fn on_schedule(&self, data_rule: String, time_rule: String) {}
//     fn on_backtest_finished(&self, indicator: Box<Indicator>) {}
//     fn on_account_status(&self, status: Box<AccountStatus>) {}
//     fn on_error(&self, error_code: String, error_msg: String) {}
//     fn on_stop(&self) {}
//     fn on_market_data_connected(&self) {}
//     fn on_trade_data_connected(&self) {}
//     fn on_market_data_disconnected(&self) {}
//     fn on_trade_data_disconnected(&self) {}
// }

// impl DefaultImpl {
//     pub fn default() -> RefCell<DefaultImpl>
//     where
//         Self: Sized,
//     {
//         RefCell::new(DefaultImpl {})
//     }
// }

pub struct CallbackImpl {
    call: Option<Rc<RefCell<dyn CallbackTrait>>>,
}

impl std::clone::Clone for CallbackImpl {
    fn clone(&self) -> Self {
        Self {
            call: self.call.clone(),
        }
    }
}

pub fn new_callback() -> Box<CallbackImpl> {
    Box::new(CallbackImpl { call: None })
}

impl CallbackImpl {
    pub fn set_call(&mut self, call: Rc<RefCell<dyn CallbackTrait>>) {
        self.call.replace(call);
    }

    fn on_init(&self) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_init()
    }

    fn on_tick(&self, tick: Box<Tick>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_tick(tick)
    }

    fn on_bar(&self, bar: Box<Bar>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_bar(bar)
    }

    fn on_l2transaction(&self, transaction: Box<L2Transaction>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_l2transaction(transaction)
    }

    fn on_l2order(&self, order: Box<L2Order>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_l2order(order)
    }

    fn on_l2order_queue(&self, order_queue: Box<L2OrderQueue>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_l2order_queue(order_queue)
    }

    fn on_order_status(&self, order: Box<Order>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_order_status(order)
    }

    fn on_execution_report(&self, rpt: Box<ExecRpt>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_execution_report(rpt)
    }

    fn on_algo_order_status(&self, order: Box<AlgoOrder>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_algo_order_status(order)
    }

    fn on_cash(&self, cash: Box<Cash>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_cash(cash)
    }

    fn on_position(&self, pos: Box<Position>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_position(pos)
    }

    fn on_parameter(&self, param: Box<Parameter>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_parameter(param)
    }

    fn on_schedule(&self, data_rule: String, time_rule: String) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_schedule(data_rule, time_rule)
    }

    fn on_backtest_finished(&self, indicator: Box<Indicator>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_backtest_finished(indicator)
    }

    fn on_account_status(&self, status: Box<AccountStatus>) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_account_status(status)
    }

    fn on_error(&self, error_code: String, error_msg: String) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_error(error_code, error_msg)
    }

    fn on_stop(&self) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_stop()
    }

    fn on_market_data_connected(&self) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_market_data_connected()
    }

    fn on_trade_data_connected(&self) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_trade_data_connected()
    }

    fn on_market_data_disconnected(&self) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_market_data_disconnected()
    }

    fn on_trade_data_disconnected(&self) {
        let call = self.call.as_ref().unwrap();
        call.borrow().on_trade_data_disconnected()
    }
}
