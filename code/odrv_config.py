#!/usr/bin/env python3

import odrive
from odrive.enums import *
from odrive.enums import MOTOR_TYPE_HIGH_CURRENT, AXIS_STATE_MOTOR_CALIBRATION, AXIS_STATE_IDLE, \
    AXIS_STATE_ENCODER_OFFSET_CALIBRATION, CONTROL_MODE_TORQUE_CONTROL, INPUT_MODE_TORQUE_RAMP
import time

def wait_for_state(axis, requested_state, timeout=10):
    """Helper function to wait for axis to enter specified state"""
    start_time = time.time()
    while axis.current_state != requested_state:
        time.sleep(0.1)
        if time.time() - start_time > timeout:
            raise TimeoutError(f"Timeout waiting for state {requested_state}")

def reconnect_to_odrive():
    """Helper function to reconnect to ODrive after reboot"""
    print("Attempting to reconnect...")
    attempts = 0
    while attempts < 3:  # Try up to 3 times
        try:
            odrv0 = odrive.find_any()
            test = odrv0.vbus_voltage
            print(f"Reconnection successful. Bus voltage: {test}V")
            return odrv0
        except:
            print(f"Reconnection attempt {attempts + 1} failed, retrying...")
            time.sleep(2)
            attempts += 1
    
    raise Exception("Failed to reconnect to ODrive after multiple attempts")

def configure_odrive():
    print("Looking for ODrive...")
    
    try:
        odrv0 = odrive.find_any()
        print("Found ODrive")
    except:
        raise Exception("No ODrive found. Is it connected?")

    # Erase configuration
    try:
        odrv0.erase_configuration()
        print("ODrive is rebooting after erase...")
        time.sleep(5)
    except:
        print("Erase configuration failed - this is normal if connection was lost")
    
    odrv0 = reconnect_to_odrive()

    # Power configuration
    print("Configuring power parameters...")
    odrv0.config.brake_resistance = 2
    odrv0.config.dc_max_positive_current = 18
    odrv0.config.dc_bus_undervoltage_trip_level = 8.0
    odrv0.config.dc_bus_overvoltage_trip_level = 56.0
    odrv0.config.dc_max_negative_current = -3.0
    odrv0.config.max_regen_current = 0
    odrv0.save_configuration()

    print("Rebooting ODrive...")
    try:
        odrv0.reboot()
    except:
        print("Reboot command failed - this is normal")
    time.sleep(5)
    odrv0 = reconnect_to_odrive()

    # Motor configuration
    print("Configuring motor parameters...")
    odrv0.axis0.motor.config.pole_pairs = 7
    odrv0.axis0.motor.config.torque_constant = 8.27 / 1200
    odrv0.axis0.motor.config.current_lim = 35
    odrv0.axis0.motor.config.calibration_current = 15
    odrv0.axis0.motor.config.resistance_calib_max_voltage = 2
    odrv0.axis0.motor.config.motor_type = MOTOR_TYPE_HIGH_CURRENT
    odrv0.axis0.motor.config.requested_current_range = 35
    odrv0.axis0.encoder.config.cpr = 8192
    odrv0.axis0.config.calibration_lockin.current = 15
    odrv0.save_configuration()

    print("Rebooting ODrive...")
    try:
        odrv0.reboot()
    except:
        print("Reboot command failed - this is normal")
    time.sleep(5)
    odrv0 = reconnect_to_odrive()

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
    try:
        odrv0.reboot()
    except:
        print("Reboot command failed - this is normal")
    time.sleep(5)
    odrv0 = reconnect_to_odrive()

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
    try:
        odrv0.reboot()
    except:
        print("Reboot command failed - this is normal")
    time.sleep(5)
    odrv0 = reconnect_to_odrive()

    # Final configuration
    odrv0.axis0.config.startup_encoder_index_search = False
    odrv0.axis0.encoder.config.use_index = False
    odrv0.axis0.encoder.config.pre_calibrated = False
    odrv0.save_configuration()

    print("Rebooting ODrive...")
    try:
        odrv0.reboot()
    except:
        print("Reboot command failed - this is normal")
    time.sleep(5)
    odrv0 = reconnect_to_odrive()
    
    print("Setup complete!")
    return odrv0

if __name__ == "__main__":
    try:
        odrv = configure_odrive()
    except Exception as e:
        print(f"Setup failed with error: {str(e)}")