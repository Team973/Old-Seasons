/*
 * intake.hpp
 *
 *  Created on: Jan 15, 2015
 *      Author: Logan Wilt
 */

#ifndef INTAKE_HPP_
#define INTAKE_HPP_

namespace frc973 {

class Intake {
public:
	Intake(VictorSP *rightMotor_, VictorSP *leftMotor_);
    void update();
    void setIntake(float indicatedSpeed);
	virtual ~Intake();
private:
    float motorSpeed;
    VictorSP *rightMotor;
    VictorSP *leftMotor;
};

} /* namespace frc973 */

#endif /* INTAKE_HPP_ */
