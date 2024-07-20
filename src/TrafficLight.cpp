#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename Type>
Type MessageQueue<Type>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> uLock(m_mutex);
    m_conditionVariable.wait(uLock, [this] { return !m_queue.empty(); }); // pass unique lock to condition variable

    // remove last vector element from queue
    Type msg = std::move(m_queue.back());
    m_queue.pop_back();

    return msg; // will not be copied due to return value optimization (RVO) in C++
}

template <typename Type>
void MessageQueue<Type>::send(Type &&message)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    
    std::lock_guard<std::mutex> uLock(m_mutex);
    m_queue.clear();
  
    // add msg to queue
    m_queue.push_back(std::move(message));
    m_conditionVariable.notify_one(); // notify client after pushing new Vehicle into vector
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    m_sharedPtr_queue = std::make_shared<MessageQueue<TrafficLightPhase>>();
    m_currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {
        if(m_sharedPtr_queue->receive() == TrafficLightPhase::green)
        return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return m_currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    
    // Initialize the random number generator and distribution
    std::random_device randomDevice;
    std::mt19937 randomEngine(randomDevice()); // Random engine seeded with randomDevice()
    std::uniform_real_distribution<> distribution(4000.0, 6000.0);

    // Record the initial time    
    std::chrono::time_point<std::chrono::system_clock> lastUpdateTime = std::chrono::system_clock::now();
    double intervalThreshold = distribution(randomEngine);

    while (true)
    {   
        // Calculate elapsed time since the last update
        auto currentTime = std::chrono::system_clock::now();
        long elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdateTime).count();
        
        if (elapsedMilliseconds >= intervalThreshold)
        {
            // Toggle traffic light [RED - GREEN]
            m_currentPhase = m_currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;
            
            // Update method
            m_sharedPtr_queue->send(std::move(m_currentPhase));
            
            // Update the last update time
            lastUpdateTime = currentTime;
            
            // Generate a new random interval threshold
            intervalThreshold = distribution(randomEngine);
        }
        
        // Sleep for a short duration to prevent busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}