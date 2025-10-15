#include "animation_controller.h"
#include <algorithm>
#include <iostream>
#include <imgui.h>
#include "robot_arm.h"

AnimationController::Keyframe::Keyframe() 
    : timestamp(0.0f), name("Unnamed") {
    jointAngles.fill(0.0f);
}

AnimationController::Keyframe::Keyframe(const std::array<float, 6>& angles, float time, const std::string& frameName)
    : jointAngles(angles), timestamp(time), name(frameName) {}

AnimationController::AnimationController() 
    : currentTime(0.0f),
      totalDuration(10.0f),
      playbackSpeed(1.0f),
      playing(false),
      looping(false),
      interpolationMethod(LINEAR),
      currentKeyframeIndex(0),
      wasPlaying(false) {}

void AnimationController::update(float deltaTime, RobotArm& robot) {
    if (!playing) return;
    
    currentTime += deltaTime * playbackSpeed;
    
    if (looping && currentTime > totalDuration) {
        currentTime = 0.0f;
        if (onStart) onStart();
    }
    
    if (!looping && currentTime > totalDuration) {
        currentTime = totalDuration;
        playing = false;
        if (onEnd) onEnd();
        return;
    }
    
    checkKeyframeEvents(currentTime);
    
    std::array<float, 6> angles;
    
    switch (interpolationMethod) {
        case LINEAR:
            angles = interpolateLinear(currentTime);
            break;
        case SMOOTHSTEP:
            angles = interpolateSmoothstep(currentTime);
            break;
        case BEZIER:
            angles = interpolateBezier(currentTime);
            break;
        default:
            angles = interpolateLinear(currentTime);
            break;
    }
    
    robot.setJointAngles(angles, false);
}

void AnimationController::renderGUI() {
    ImGui::Begin("Animation Controller");
    
    if (ImGui::Button(playing ? "Pause" : "Play")) {
        if (playing) {
            pause();
        } else {
            play();
        }
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Stop")) {
        stop();
    }
    
    ImGui::SameLine();
    ImGui::Checkbox("Loop", &looping);
    
    ImGui::SliderFloat("Speed", &playbackSpeed, 0.1f, 5.0f);
    ImGui::SliderFloat("Time", &currentTime, 0.0f, totalDuration);
    ImGui::InputFloat("Duration (s)", &totalDuration);
    
    const char* methods[] = { "Linear", "Smoothstep", "Bezier" };
    ImGui::Combo("Interpolation", (int*)&interpolationMethod, methods, IM_ARRAYSIZE(methods));
    
    ImGui::Separator();
    ImGui::Text("Keyframes:");
    
    for (size_t i = 0; i < keyframes.size(); i++) {
        ImGui::PushID(i);
        
        ImGui::Text("%s: %.2fs", keyframes[i].name.c_str(), keyframes[i].timestamp);
        
        if (ImGui::Button("Go To")) {
            currentTime = keyframes[i].timestamp;
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Delete")) {
            removeKeyframe(i);
            ImGui::PopID();
            break;
        }
        
        ImGui::PopID();
    }
    
    if (ImGui::Button("Add Keyframe")) {
        Keyframe newFrame;
        newFrame.timestamp = currentTime;
        newFrame.name = "Keyframe " + std::to_string(keyframes.size() + 1);
        addKeyframe(newFrame);
    }
    
    ImGui::Separator();
    if (ImGui::Button("Validate Trajectory")) {
        if (validateTrajectory()) {
            ImGui::Text("Trajectory is valid!");
        } else {
            ImGui::Text("Trajectory has issues!");
        }
    }
    
    ImGui::End();
}

void AnimationController::addKeyframe(const Keyframe& keyframe) {
    keyframes.push_back(keyframe);
    sortKeyframes();
}

void AnimationController::removeKeyframe(int index) {
    if (index >= 0 && index < keyframes.size()) {
        keyframes.erase(keyframes.begin() + index);
    }
}

void AnimationController::clearKeyframes() {
    keyframes.clear();
}

void AnimationController::play() {
    if (!playing) {
        playing = true;
        if (currentTime >= totalDuration) {
            currentTime = 0.0f;
        }
        if (onStart) onStart();
    }
}

void AnimationController::pause() {
    playing = false;
}

void AnimationController::stop() {
    playing = false;
    currentTime = 0.0f;
    if (onEnd) onEnd();
}

bool AnimationController::validateTrajectory() const {
    if (keyframes.size() < 2) return true;
    
    auto sampled = getSampledTrajectory(100);
    const std::array<RobotJoint, 6>& joints = RobotArm().getJoints();
    
    for (const auto& angles : sampled) {
        for (int i = 0; i < 6; i++) {
            if (angles[i] < joints[i].getMinAngle() || angles[i] > joints[i].getMaxAngle()) {
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::array<float, 6>> AnimationController::getSampledTrajectory(int samples) const {
    std::vector<std::array<float, 6>> trajectory;
    
    if (keyframes.size() < 2) return trajectory;
    
    float timeStep = totalDuration / samples;
    
    for (int i = 0; i <= samples; i++) {
        float time = i * timeStep;
        std::array<float, 6> angles;
        
        switch (interpolationMethod) {
            case LINEAR:
                angles = interpolateLinear(time);
                break;
            case SMOOTHSTEP:
                angles = interpolateSmoothstep(time);
                break;
            case BEZIER:
                angles = interpolateBezier(time);
                break;
            default:
                angles = interpolateLinear(time);
                break;
        }
        
        trajectory.push_back(angles);
    }
    
    return trajectory;
}

std::array<float, 6> AnimationController::interpolateLinear(float time) const {
    if (keyframes.empty()) return std::array<float, 6>();
    if (keyframes.size() == 1) return keyframes[0].jointAngles;
    
    int idx = findKeyframeIndex(time);
    if (idx < 0 || idx >= keyframes.size() - 1) return keyframes[0].jointAngles;
    
    const Keyframe& k0 = keyframes[idx];
    const Keyframe& k1 = keyframes[idx + 1];
    
    float t = (time - k0.timestamp) / (k1.timestamp - k0.timestamp);
    t = glm::clamp(t, 0.0f, 1.0f);
    
    std::array<float, 6> result;
    for (int i = 0; i < 6; i++) {
        result[i] = glm::mix(k0.jointAngles[i], k1.jointAngles[i], t);
    }
    
    return result;
}

std::array<float, 6> AnimationController::interpolateSmoothstep(float time) const {
    if (keyframes.empty()) return std::array<float, 6>();
    if (keyframes.size() == 1) return keyframes[0].jointAngles;
    
    int idx = findKeyframeIndex(time);
    if (idx < 0 || idx >= keyframes.size() - 1) return keyframes[0].jointAngles;
    
    const Keyframe& k0 = keyframes[idx];
    const Keyframe& k1 = keyframes[idx + 1];
    
    float t = (time - k0.timestamp) / (k1.timestamp - k0.timestamp);
    t = glm::clamp(t, 0.0f, 1.0f);
    
    float smoothT = t * t * (3.0f - 2.0f * t);
    
    std::array<float, 6> result;
    for (int i = 0; i < 6; i++) {
        result[i] = glm::mix(k0.jointAngles[i], k1.jointAngles[i], smoothT);
    }
    
    return result;
}

std::array<float, 6> AnimationController::interpolateBezier(float time) const {
    if (keyframes.size() < 3) return interpolateSmoothstep(time);
    
    int idx = findKeyframeIndex(time);
    if (idx < 1 || idx >= keyframes.size() - 2) return interpolateSmoothstep(time);
    
    const Keyframe& k0 = keyframes[idx - 1];
    const Keyframe& k1 = keyframes[idx];
    const Keyframe& k2 = keyframes[idx + 1];
    const Keyframe& k3 = keyframes[idx + 2];
    
    float t = (time - k1.timestamp) / (k2.timestamp - k1.timestamp);
    t = glm::clamp(t, 0.0f, 1.0f);
    
    float u = 1.0f - t;
    float uu = u * u;
    float uuu = uu * u;
    float tt = t * t;
    float ttt = tt * t;
    
    std::array<float, 6> result;
    for (int i = 0; i < 6; i++) {
        float p0 = k0.jointAngles[i];
        float p1 = k1.jointAngles[i] + (k2.jointAngles[i] - k0.jointAngles[i]) / 6.0f;
        float p2 = k2.jointAngles[i] - (k3.jointAngles[i] - k1.jointAngles[i]) / 6.0f;
        float p3 = k3.jointAngles[i];
        
        result[i] = uuu * p0 + 3 * uu * t * p1 + 3 * u * tt * p2 + ttt * p3;
    }
    
    return result;
}

void AnimationController::sortKeyframes() {
    std::sort(keyframes.begin(), keyframes.end(), 
        [](const Keyframe& a, const Keyframe& b) {
            return a.timestamp < b.timestamp;
        });
}

int AnimationController::findKeyframeIndex(float time) const {
    if (keyframes.empty() || time < keyframes[0].timestamp) return -1;
    
    for (size_t i = 0; i < keyframes.size() - 1; i++) {
        if (time >= keyframes[i].timestamp && time < keyframes[i + 1].timestamp) {
            return i;
        }
    }
    
    return keyframes.size() - 1;
}

bool AnimationController::isTimeValid(float time) const {
    return time >= 0.0f && time <= totalDuration;
}

void AnimationController::checkKeyframeEvents(float time) {
    for (size_t i = 0; i < keyframes.size(); i++) {
        if (std::abs(time - keyframes[i].timestamp) < 0.01f && 
            (i != currentKeyframeIndex || !wasPlaying)) {
            currentKeyframeIndex = i;
            if (onKeyframe) onKeyframe(i);
            break;
        }
    }
    
    wasPlaying = playing;
}
