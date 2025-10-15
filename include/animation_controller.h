#ifndef ANIMATION_CONTROLLER_H
#define ANIMATION_CONTROLLER_H

#include <vector>
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include "robot_arm.h"

class AnimationController {
public:
    struct Keyframe {
        std::array<float, 6> jointAngles;
        float timestamp;
        std::string name;
        
        Keyframe();
        Keyframe(const std::array<float, 6>& angles, float time, const std::string& frameName = "");
    };
    
    enum InterpolationMethod {
        LINEAR,
        SMOOTHSTEP,
        BEZIER
    };
    
    AnimationController();
    
    void update(float deltaTime, RobotArm& robot);
    void renderGUI();
    
    void addKeyframe(const Keyframe& keyframe);
    void removeKeyframe(int index);
    void clearKeyframes();
    const std::vector<Keyframe>& getKeyframes() const { return keyframes; }
    
    void play();
    void pause();
    void stop();
    void setLooping(bool loop) { looping = loop; }
    bool isPlaying() const { return playing; }
    bool isLooping() const { return looping; }
    
    void setDuration(float duration) { totalDuration = duration; }
    float getDuration() const { return totalDuration; }
    float getCurrentTime() const { return currentTime; }
    void setPlaybackSpeed(float speed) { playbackSpeed = speed; }
    float getPlaybackSpeed() const { return playbackSpeed; }
    
    void setInterpolationMethod(InterpolationMethod method) { interpolationMethod = method; }
    InterpolationMethod getInterpolationMethod() const { return interpolationMethod; }
    
    bool validateTrajectory() const;
    std::vector<std::array<float, 6>> getSampledTrajectory(int samples = 100) const;
    
    void setOnAnimationStart(std::function<void()> callback) { onStart = callback; }
    void setOnAnimationEnd(std::function<void()> callback) { onEnd = callback; }
    void setOnKeyframeReached(std::function<void(int)> callback) { onKeyframe = callback; }
    
private:
    std::vector<Keyframe> keyframes;
    float currentTime;
    float totalDuration;
    float playbackSpeed;
    bool playing;
    bool looping;
    InterpolationMethod interpolationMethod;
    
    std::function<void()> onStart;
    std::function<void()> onEnd;
    std::function<void(int)> onKeyframe;
    
    int currentKeyframeIndex;
    bool wasPlaying;
    
    std::array<float, 6> interpolateLinear(float time) const;
    std::array<float, 6> interpolateSmoothstep(float time) const;
    std::array<float, 6> interpolateBezier(float time) const;
    
    void sortKeyframes();
    int findKeyframeIndex(float time) const;
    bool isTimeValid(float time) const;
    void checkKeyframeEvents(float time);
};

#endif
