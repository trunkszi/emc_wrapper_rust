use crate::*;

#[cfg(test)]
#[test]
fn it_works() {
    let s = CallbackImpl::new();
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

    loop {
        std::thread::sleep(std::time::Duration::from_secs(1));
    }

    // let ss = ffi::instantiate_strategy();
    // s.s = ss;

    // let persons = ffi::get_persons();
    // println!("{:?}", persons);

    // let b = Box::new(ffi::Info {
    //     address: String::from("address"),
    //     phone: String::from("phone"),
    //     name: String::from("hello"),
    //     age: 1,
    // });

    // ffi::set_info(b);
    // let ins = &mut ffi::Info {
    //     address: String::from("address1"),
    //     phone: String::from("phone1"),
    //     name: String::from("world"),
    //     age: 1,
    // };

    // ffi::set_infos(ins);

    // let in1 = ffi::get_info();
    // println!("{:#?}", in1);

    // let in2 = ffi::get_info_box();
    // println!("{:#?}", *in2);
}
