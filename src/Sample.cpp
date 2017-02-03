/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/
#include "Sample.h"
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

const int CYLINDER_RADIUS = 1;

void SampleListener::onInit(const Controller& controller) {
    std::cout << "Initialized" << std::endl;

}

void SampleListener::onConnect(const Controller& controller) {
    std::cout << "Connected" << std::endl;
    controller.enableGesture(Gesture::TYPE_CIRCLE);
    controller.enableGesture(Gesture::TYPE_KEY_TAP);
    controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
    controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
    // Note: not dispatched when running in a debugger.
    if(print_leap_stats)
    {
        std::cout << "Disconnected" << std::endl;
    }
}

void SampleListener::onExit(const Controller& controller) {
    if(print_leap_stats) std::cout << "Exited" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {

    // Get the most recent frame and report some basic information
    const Frame frame = controller.frame();
    if(print_leap_stats) 
    {
        std::cout << "Frame id: " << frame.id()
                            << ", timestamp: " << frame.timestamp()
                            << ", hands: " << frame.hands().count()
                            << ", extended fingers: " << frame.fingers().extended().count()
                            << ", tools: " << frame.tools().count()
                            << ", gestures: " << frame.gestures().count() << std::endl;
    }

    HandList hands = frame.hands();
    for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
        // Get the first hand
        const Hand hand = *hl;
        std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
        if(print_leap_stats)
        {
            std::cout << std::string(2, ' ') << handType << ", id: " << hand.id()
                                << ", palm position: " << hand.palmPosition() << std::endl;
        }

        // Get the hand's normal vector and direction
        const Vector normal = hand.palmNormal();
        const Vector direction = hand.direction();


        if(print_leap_stats)
        {
            // Calculate the hand's pitch, roll, and yaw angles
            std::cout << std::string(2, ' ') <<  "pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
                                << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
                                << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;            
        }

        // Get the Arm bone
        Arm arm = hand.arm();
        if(print_leap_stats)
        {
            std::cout << std::string(2, ' ') <<  "Arm direction: " << arm.direction()
                                << " wrist position: " << arm.wristPosition()
                                << " elbow position: " << arm.elbowPosition() << std::endl;
        }

        // Get fingers
        const FingerList fingers = hand.fingers();
        for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {

            const Finger finger = *fl;

            if(print_leap_stats)
            {
                std::cout << std::string(4, ' ') <<  fingerNames[finger.type()]
                                    << " finger, id: " << finger.id()
                                    << ", length: " << finger.length()
                                    << "mm, width: " << finger.width() << std::endl;
            }

            // Get finger bones
            for (int b = 0; b < 4; ++b) {
                Bone::Type boneType = static_cast<Bone::Type>(b);
                Bone bone = finger.bone(boneType);
                if(print_leap_stats)
                {
                    std::cout << std::string(6, ' ') <<  boneNames[boneType]
                                        << " bone, start: " << bone.prevJoint()
                                        << ", end: " << bone.nextJoint()
                                        << ", direction: " << bone.direction() << std::endl;
                }
            }
        }
    }

    // Get tools
    const ToolList tools = frame.tools();
    for (ToolList::const_iterator tl = tools.begin(); tl != tools.end(); ++tl) {
        const Tool tool = *tl;
        if(print_leap_stats)
        {
            std::cout << std::string(2, ' ') <<  "Tool, id: " << tool.id()
                                << ", position: " << tool.tipPosition()
                                << ", direction: " << tool.direction() << std::endl;
        }
    }

    // Get gestures
    const GestureList gestures = frame.gestures();
    for (int g = 0; g < gestures.count(); ++g) {
        Gesture gesture = gestures[g];

        switch (gesture.type()) {
            case Gesture::TYPE_CIRCLE:
            {                
                // printf("Circle gesture\n");

                CircleGesture circle = gesture;
                std::string clockwiseness;

                if (circle.pointable().direction().angleTo(circle.normal()) <= PI/2) {
                    clockwiseness = "clockwise";
                } else {
                    clockwiseness = "counterclockwise";
                }

                // Calculate angle swept since last frame
                float sweptAngle = 0;
                if (circle.state() != Gesture::STATE_START) {
                    CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
                    sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
                }
                if(print_leap_stats)
                {
                    std::cout << std::string(2, ' ')
                                        << "Circle id: " << gesture.id()
                                        << ", state: " << stateNames[gesture.state()]
                                        << ", progress: " << circle.progress()
                                        << ", radius: " << circle.radius()
                                        << ", angle " << sweptAngle * RAD_TO_DEG
                                        <<  ", " << clockwiseness << std::endl;
                }
                break;
            }
            case Gesture::TYPE_SWIPE:
            {
                // reset = true;s


                SwipeGesture swipe = gesture;
                if(print_leap_stats)
                {
                    std::cout << std::string(2, ' ')
                        << "Swipe id: " << gesture.id()
                        << ", state: " << stateNames[gesture.state()]
                        << ", direction: " << swipe.direction()
                        << ", speed: " << swipe.speed() << std::endl;
                }
                break;
            }
            case Gesture::TYPE_KEY_TAP:
            {
                KeyTapGesture tap = gesture;
                if(print_leap_stats)
                {
                    std::cout << std::string(2, ' ')
                        << "Key Tap id: " << gesture.id()
                        << ", state: " << stateNames[gesture.state()]
                        << ", position: " << tap.position()
                        << ", direction: " << tap.direction()<< std::endl;
                }
                break;
            }
            case Gesture::TYPE_SCREEN_TAP:
            {
                ScreenTapGesture screentap = gesture;
                if(print_leap_stats)
                {
                    std::cout << std::string(2, ' ')
                        << "Screen Tap id: " << gesture.id()
                        << ", state: " << stateNames[gesture.state()]
                        << ", position: " << screentap.position()
                        << ", direction: " << screentap.direction()<< std::endl;
                }
                break;
            }
            default:
            {
                if(print_leap_stats)
                {
                    std::cout << std::string(2, ' ')  << "Unknown gesture type." << std::endl;
                }
                break;
            }
        }
    }

}

void SampleListener::onFocusGained(const Controller& controller) {
    if(print_leap_stats) std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
    if(print_leap_stats) std::cout << "Focus Lost" << std::endl;
}

void SampleListener::onDeviceChange(const Controller& controller) {
    if(print_leap_stats) std::cout << "Device Changed" << std::endl;
    const DeviceList devices = controller.devices();

    for (int i = 0; i < devices.count(); ++i) {
        if(print_leap_stats) std::cout << "id: " << devices[i].toString() << std::endl;
        if(print_leap_stats) std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
    }
}

void SampleListener::onServiceConnect(const Controller& controller) {
    if(print_leap_stats) std::cout << "Service Connected" << std::endl;
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
    if(print_leap_stats) std::cout << "Service Disconnected" << std::endl;
}

int main(int argc, char** argv) {
    // Create a sample listener and controller
    SampleListener listener;
    Controller controller;

    // Have the sample listener receive events from the controller
    controller.addListener(listener);

    controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    // Remove the sample listener when done
    controller.removeListener(listener);

    return 0;
}
