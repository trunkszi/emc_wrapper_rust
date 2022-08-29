use cxx;
use std::cell::RefCell;
use std::sync::mpsc::channel;
use std::rc::Rc;
use wrapper::*;
use ta_lib::*;

pub struct Strategy {
    inner: cxx::SharedPtr<wrapper::ffi::strategy>,
    callback: Option<Box<wrapper::CallbackImpl>>,
}

impl wrapper::CallbackTrait for Strategy {
    fn on_init(&self) {
        self.inner.subscribe(
            &mut String::from("SHSE.603501"),
            &mut String::from("tick"),
            false,
        );
    }
    fn on_tick(&self, tick: Box<Tick>) {
        println!("{:#?}", tick);
    }
    fn on_bar(&self, bar: Box<Bar>) {
        println!("{:#?}", bar);
    }
    fn on_l2transaction(&self, transaction: Box<L2Transaction>) {
        println!("{:#?}", transaction);
    }
    fn on_l2order(&self, order: Box<L2Order>) {
        println!("{:#?}", order);
    }
    fn on_l2order_queue(&self, order_queue: Box<L2OrderQueue>) {
        println!("{:#?}", order_queue);
    }
    fn on_order_status(&self, order: Box<Order>) {
        println!("{:#?}", order);
    }
    fn on_execution_report(&self, rpt: Box<ExecRpt>) {
        println!("{:#?}", rpt);
    }
    fn on_algo_order_status(&self, order: Box<AlgoOrder>) {
        println!("{:#?}", order);
    }
    fn on_cash(&self, cash: Box<Cash>) {
        println!("{:#?}", cash);
    }
    fn on_position(&self, pos: Box<Position>) {
        println!("{:#?}", pos);
    }
    fn on_parameter(&self, param: Box<Parameter>) {
        println!("{:#?}", param);
    }
    fn on_schedule(&self, data_rule: String, time_rule: String) {
        println!("{:#?}", data_rule);
        println!("{:#?}", time_rule);
    }
    fn on_backtest_finished(&self, indicator: Box<Indicator>) {
        println!("{:#?}", indicator);
    }
    fn on_account_status(&self, status: Box<AccountStatus>) {
        println!("{:#?}", status);
    }
    fn on_error(&self, error_code: String, error_msg: String) {
        println!("{:#?}", error_code);
        println!("{:#?}", error_msg);
    }
    fn on_stop(&self) {}
    fn on_market_data_connected(&self) {}
    fn on_trade_data_connected(&self) {}
    fn on_market_data_disconnected(&self) {}
    fn on_trade_data_disconnected(&self) {}
}

impl Clone for Strategy {
    fn clone(&self) -> Self {
        Strategy {
            inner: self.inner.clone(),
            callback: self.callback.clone(),
        }
    }
}

impl Strategy {
    pub fn new() -> Rc<RefCell<Strategy>> {
        let ptr = wrapper::ffi::instantiate_strategy();
        let mut call = ptr.get_callback_instantiate();
        let s = Rc::new(RefCell::new(Strategy {
            inner: ptr,
            callback: None,
        }));

        call.set_call(s.clone());
        s.borrow_mut().callback.replace(call);
        s
    }
}

fn main() {
    let s = Strategy::new();
    let s = s.borrow();
    s.inner
        .set_strategy_id(&mut String::from("016fef2e-192f-11ed-9e30-d45d64a6d10d"));
    s.inner.set_token(&mut String::from(
        "1a0119ff3c508a2bd60ff086c577cb2c45ffe0d0",
    ));
    s.inner.set_mode(2);
    s.inner.set_backtest_config(
        &mut String::from("2022-08-26 09:00:00"),
        &mut String::from("2022-08-26 16:00:00"),
        10000000.0,
        1.0,
        0.0001,
        0.0001,
        1,
        1,
    );
    s.inner.run();

    let (tx, rx) = channel();

    ctrlc::set_handler(move ||
        tx.send(()).expect("Could not send signal on channel."))
        .expect("Error setting Ctrl-C handler");


    rx.recv().expect("Could not receive from channel.");
    s.inner.stop();
    println!("Got it! Exiting...");
    // loop {
    //     std::thread::sleep(std::time::Duration::from_secs(1));
    // }
}
