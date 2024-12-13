#!/usr/bin/env python3

import odrive
from odrive.enums import *
import time

def wait_for_state(axis, requested_state, timeout=10):
    """Helper function to wait for axis to enter specified state"""
    start_time = time.time()
    while axis.current_state != requested_state:
        time.sleep(0.1)
        if time.time() - start_time > timeout:
            raise TimeoutError(f"Timeout waiting for state {requested_state}")

def configure_odrive():
    print("Looking for ODrive...")
    odrv0 = odrive.find_any()
    print("ODrive found!")

    # Basic configuration
    print("Erasing existing configuration...")
    odrv0.erase_configuration()
    
    # Power configuration
    print("Configuring power parameters...")
    odrv0.config.brake_resistance = 2
    odrv0.config.dc_max_positive_current = 18
    odrv0.config.dc_bus_undervoltage_trip_level = 8.0
    odrv0.config.dc_bus_overvoltage_trip_level = 56.0
    odrv0.config.dc_max_negative_current = -3.0
    odrv0.config.max_regen_current = 0
    odrv0.save_configuration()

    # Motor configuration
    print("Configuring motor parameters...")
    odrv0.axis0.motor.config.pole_pairs = 7
    odrv0.axis0.motor.config.torque_constant = 8.27 / 1200
    odrv0.axis0.motor.config.current_lim = 35
    odrv0.axis0.motor.config.calibration_current = 15
    odrv0.axis0.motor.config.resistance_calib_max_voltage = 2
    odrv0.axis0.motor.config.motor_type = MOTOR_TYPE_HIGH_CURRENT
    odrv0.axis0.motor.config.requested_current_range = 35
    
    # Encoder configuration
    odrv0.axis0.encoder.config.cpr = 8192
    odrv0.save_configuration()

    # Controller configuration
    print("Configuring controller parameters...")
    odrv0.axis0.controller.config.control_mode = CONTROL_MODE_TORQUE_CONTROL
    odrv0.axis0.controller.config.vel_limit = 50
    odrv0.axis0.controller.config.pos_gain = 30
    odrv0.axis0.controller.config.vel_gain = 0.008
    odrv0.axis0.controller.config.vel_integrator_gain = 0.05
    odrv0.axis0.controller.config.input_mode = INPUT_MODE_TORQUE_RAMP
    odrv0.axis0.controller.config.torque_ramp_rate = 100
    odrv0.save_configuration()
    
    print("Rebooting ODrive...")
    odrv0.reboot()
    time.sleep(5)  # Wait for reboot
    odrv0 = odrive.find_any()  # Reconnect after reboot

    # Motor calibration
    print("Starting motor calibration...")
    odrv0.axis0.requested_state = AXIS_STATE_MOTOR_CALIBRATION
    wait_for_state(odrv0.axis0, AXIS_STATE_IDLE)
    
    odrv0.axis0.motor.config.pre_calibrated = True
    odrv0.save_configuration()

    # Encoder calibration
    print("Starting encoder offset calibration...")
    odrv0.axis0.requested_state = AXIS_STATE_ENCODER_OFFSET_CALIBRATION
    wait_for_state(odrv0.axis0, AXIS_STATE_IDLE)

    # Startup configuration
    odrv0.axis0.config.startup_encoder_offset_calibration = True
    odrv0.axis0.config.startup_closed_loop_control = False
    odrv0.axis0.encoder.config.use_index = False
    odrv0.save_configuration()
    
    print("Rebooting ODrive...")
    odrv0.reboot()
    time.sleep(5)
    odrv0 = odrive.find_any()

    # Final configuration
    odrv0.axis0.config.startup_encoder_index_search = False
    odrv0.axis0.encoder.config.use_index = False
    odrv0.axis0.encoder.config.pre_calibrated = False
    odrv0.save_configuration()
    
    print("Setup complete!")
    return odrv0

if __name__ == "__main__":
    try:
        odrv = configure_odrive()
    except Exception as e:
        print(f"Setup failed with error: {str(e)}")