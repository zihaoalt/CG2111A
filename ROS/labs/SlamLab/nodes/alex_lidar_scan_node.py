# This node is an example of a simple publisher that publishes the scan data from the Lidar to the "lidar/scan" topic.
# The node connects to a Lidar device, configures it, and starts scanning. It publishes the status and scan data to the "lidar/status" and "lidar/scan" topics, respectively.

# Import Python Native Modules. We require the Barrier class from threading to synchronize the start of multiple threads.
from threading import Barrier

# Import the required pubsub modules. PubSubMsg class to extract the payload from a message.
from pubsub.pub_sub_manager import ManagedPubSubRunnable, PubSubMsg
from pubsub.pub_sub_manager import publish, subscribe, unsubscribe, getMessages, getCurrentExecutionContext

# Import the required lidar modules
from lidar.alex_lidar import   lidarConnect, lidarDisconnect, process_scan, resampleLidarScan


# Constants
PORT = "/dev/ttyUSB0"   # Linux
BAUDRATE = 115200       # The default baud rate for the RPLidar A1M8
INITIAL_ROUNDS_IGNORED = 10
DEFAULT_SCAN_MODE = 4
USE_LIDAR_TYPICAL = True
MAX_LIDAR_DISTANCE = 1
MIN_LIDAR_DISTANCE = 0
MIN_LIDAR_QUALITY = 0

LIDAR_SCAN_TOPIC = "lidar/scan"
# LIDAR_STATUS_TOPIC = "lidar/status"


def lidarScanThread(setupBarrier:Barrier=None, readyBarrier:Barrier=None):
    """
    Thread function to handle Lidar scanning and data publishing.
    This function connects to a Lidar device, configures it, and starts scanning.
    It publishes scan data to the "lidar/scan" topic.
    
    Args:
        setupBarrier (Barrier, optional): A threading barrier to coordinate thread setup. Defaults to None.
        readyBarrier (Barrier, optional): A threading barrier to synchronize when the thread is ready to start scanning. Defaults to None.
    
    Raises:
        Exception: If there is an error during Lidar connection or scanning.
    
    Returns:
        None
    """

    # Setup
    ctx:ManagedPubSubRunnable = getCurrentExecutionContext()



    # Perform any setup here
    setupBarrier.wait() if readyBarrier != None else None
    
    attempts = 5
    scan_mode = DEFAULT_SCAN_MODE
    scan_mode_info = None
    us_per_sample = None
    sample_rates = None

    # Attempt to connect to Lidar
    for i in range(attempts):
        try:
            print(f"Connecting to Lidar... Attempt {i+1}/{attempts}")
            lidar = lidarConnect( port=PORT, baudrate=BAUDRATE)
            lidar.set_motor_pwm(500)

            # health
            health = lidar.get_health()

            scan_mode = lidar.get_scan_mode_typical() if USE_LIDAR_TYPICAL else DEFAULT_SCAN_MODE
            scan_mode_info = lidar.get_scan_modes()[scan_mode]

            # sampling rates
            sr = lidar.get_samplerate()
            sample_rates = sr

            if all([x != None for x in [health, scan_mode_info, sr]]):
                break
        except Exception as e:
            pass

    if not scan_mode_info:
        # Failed to connect to Lidar, trigger system to shut down
        print("Failed to connect to Lidar")
        readyBarrier.wait() if readyBarrier != None else None
        ctx.doExit()
        return 
    
    us_per_sample = scan_mode_info.us_per_sample

    lidarStatusString = f"\nLidar Connected!"
    lidarStatusString += f"\nHealth: {health}"
    lidarStatusString += f"\nScan Mode: {scan_mode_info.name}"
    lidarStatusString += f"\nMax Distance (m): {scan_mode_info.max_distance}"
    lidarStatusString += f"\nUs per sample: {us_per_sample}"
    lidarStatusString += f"\nFsample: {1e6/us_per_sample} Hz"
    print(lidarStatusString)

    # Wait for all Threads ready
    readyBarrier.wait() if readyBarrier != None else None

    import numpy as np

    # Receiving Logic Loop
    try:
        scan_generator = lidar.start_scan_express(scan_mode)
        current_round = {"r":0, "buff":[], "doScan":False}
        for count, scan in enumerate(scan_generator()):
            current_round, results = process_scan((count,scan), scanState=current_round)
            if results and current_round["r"] > INITIAL_ROUNDS_IGNORED:
                # Filter out points that are too close or too far, or have low quality
                # def filter_lidar_scan(data, min_dist, max_dist, min_quality):
                #    return [pt for pt in data if min_dist <= pt[1] <= max_dist and pt[2] >= min_quality]

                #filtered_results = filter_lidar_scan(results, MIN_LIDAR_DISTANCE, MAX_LIDAR_DISTANCE, MIN_LIDAR_QUALITY)
            
                #print(np.shape(results))
                #print(np.shape(filtered_results))

                # TODO: [Optional] Filter your results to reject low quality scans
                # process_scan provides angle, distance and quality information
                # The quality information can be used to filter out low quality scans
                # You can filter the results based on the quality information here
                # Or choose to filter the results elsewhere in the processing pipeline

                #publish(LIDAR_SCAN_TOPIC, filtered_results)
                publish(LIDAR_SCAN_TOPIC, results)

            if ctx.isExit():
                break

    except KeyboardInterrupt:
        pass
    except Exception as e:
        print(f"Error: {e}")
        pass

    # Shutdown and exit the thread gracefully
    print("Exiting Lidar Scan Thread")
    ctx.doExit()
    pass

