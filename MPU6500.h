#pragma once
#include <Arduino.h>
#include<Wire.h>

class MPU6500 {
  public:
    MPU6500(int16_t mpu_addr=0x68) {
        addr=mpu_addr;
        Wire.begin();
        Wire.setClock(400000UL); // Set I2C frequency to 400kHz
        Wire.beginTransmission(mpu_addr);
        Wire.write(0x6B);
        Wire.write(0); 
        Wire.endTransmission(true);
    }

    double Gyro_X() {
        Wire.beginTransmission(addr);
        Wire.write(0x43);
        Wire.endTransmission(false);
        Wire.requestFrom(addr,2);
        double GyX=Wire.read()<<8|Wire.read();
        Wire.endTransmission(true);
        return GyX;
    }

    double Gyro_Y() {
        Wire.beginTransmission(addr);
        Wire.write(0x43);
        Wire.endTransmission(false);
        Wire.requestFrom(addr,2);
        double GyY=Wire.read()<<8|Wire.read();
        Wire.endTransmission(true);
        return GyY;
    }

    double Gyro_Z() {
        Wire.beginTransmission(addr);
        Wire.write(0x43);
        Wire.endTransmission(false);
        Wire.requestFrom(addr,2);
        double GyZ=Wire.read()<<8|Wire.read();
        Wire.endTransmission(true);
        return GyZ;
    }

    double Acs_X() {
        Wire.beginTransmission(addr);
        Wire.write(0x43);
        Wire.endTransmission(false);
        Wire.requestFrom(addr,2);
        double AcX=Wire.read()<<8|Wire.read();
        Wire.endTransmission(true);
        return AcX;
    }

    double Acs_Y() {
        Wire.beginTransmission(addr);
        Wire.write(0x43);
        Wire.endTransmission(false);
        Wire.requestFrom(addr,2);
        double AcY=Wire.read()<<8|Wire.read();
        Wire.endTransmission(true);
        return AcY;
    }

    double Acs_Z() {
        Wire.beginTransmission(addr);
        Wire.write(0x43);
        Wire.endTransmission(false);
        Wire.requestFrom(addr,2);
        double AcZ=Wire.read()<<8|Wire.read();
        Wire.endTransmission(true);
        return AcZ;
    }
    
    
  private:
    int16_t addr = 0x68;
    uint8_t pin_f = 0;
    uint8_t pin_b = 0;
    volatile int16_t speed = 0;
};