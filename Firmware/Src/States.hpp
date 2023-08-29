//
// Created by treys on 2023/08/28
//

#ifndef STATES_HPP
#define STATES_HPP

#include "System.hpp"
#include "tinyfsm.hpp"

struct RS0 : tinyfsm::Event {};
struct RS1 : tinyfsm::Event {};
struct RS2 : tinyfsm::Event {};
struct RS3 : tinyfsm::Event {};

/* Finite State Machine base class */
class RunStateFSM : public tinyfsm::Fsm<RunStateFSM> {
 public:
  /* react on all events and do nothing by default */
  virtual void react(RS0 const &) {}

  virtual void react(RS1 const &) {}

  virtual void react(RS2 const &) {}

  virtual void react(RS3 const &) {}

  virtual void react(tinyfsm::Event const &) {}

  virtual void entry(){};

  virtual void exit(){};

  static void reset(){};

  static auto current_state() -> uint8_t {
    return state;
  }

  static auto current_state(uint8_t new_state) -> void {
    state = new_state;
  }

  inline static uint8_t state = 0;
};

class RunState0;
class RunState1;
class RunState2;
class RunState3;

class RunState0 : public RunStateFSM {
  void entry() override {
    FMTDebug("ENTER: Run State 0\n");
    RunStateFSM::current_state(0);
    sys123::pwm_enable_led_pin.Write(rpp::gpio::Levels::high);
    sys123::pwm_smps.Disable();
  }

  void exit() override {
    FMTDebug("EXIT: Run State 0\n");
  }

  void react(RS0 const &) override {}

  void react(RS1 const &) override {
    transit<RunState1>();
  }

  void react(RS2 const &) override {
    transit<RunState2>();
  }

  void react(RS3 const &) override {
    transit<RunState3>();
  }
};

class RunState1 : public RunStateFSM {
  void entry() override {
    FMTDebug("ENTER: Run State 1\n");
    RunStateFSM::current_state(1);
  }

  void exit() override {
    FMTDebug("EXIT: Run State 1\n");
  }


  void react(RS0 const &) override {
    transit<RunState0>();
  }

  void react(RS1 const &) override {}

  void react(RS2 const &) override {
    transit<RunState2>();
  }

  void react(RS3 const &) override {
    transit<RunState3>();
  }
};

class RunState2 : public RunStateFSM {
  void entry() override {
    FMTDebug("ENTER: Run State 1\n");
    RunStateFSM::current_state(2);
    sys123::pwm_smps.Enable();
    sys123::pwm_smps.DutyCycle(sys123::duty.Load() * 10'000);
    sys123::coulomb_counter_call.Enable();
    sys123::pwm_enable_led_pin.Write(rpp::gpio::Levels::low);
  }

  void exit() override {
    FMTDebug("EXIT: Run State 1\n");
    sys123::coulomb_counter_call.Disable();
  }

  void react(RS0 const &) override {
    transit<RunState0>();
  }

  void react(RS1 const &) override {
    transit<RunState1>();
  }

  void react(RS2 const &) override {}

  void react(RS3 const &) override {
    transit<RunState3>();
  }
};

class RunState3 : public RunStateFSM {
  void entry() override {
    FMTDebug("ENTER: Run State 1\n");
    RunStateFSM::current_state(3);
    sys123::pwm_smps.Enable();
    sys123::pwm_smps.DutyCycle(sys123::duty.Load() * 10'000);
    sys123::coulomb_counter_call.Enable();
    sys123::mppt_call.Enable();
    sys123::pwm_enable_led_pin.Write(rpp::gpio::Levels::low);
  }

  void exit() override {
    FMTDebug("EXIT: Run State 1\n");
    sys123::mppt_call.Disable();
    sys123::coulomb_counter_call.Disable();
  }

  void react(RS0 const &) override {
    transit<RunState0>();
  }

  void react(RS1 const &) override {
    transit<RunState1>();
  }

  void react(RS2 const &) override {
    transit<RunState2>();
  }

  void react(RS3 const &) override {}
};

FSM_INITIAL_STATE(RunStateFSM, RunState0)

#endif  // STATES_HPP
