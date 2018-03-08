// Local
#include "run.hpp"

// Std
#include <iostream>
#include <cstdio>

namespace owl {
    void display_help() {
        std::cout << "Usage: owl [OPTIONS]" << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "  --help             Display this help message" << std::endl;
        std::cout << "  --video-url <url>  Specify the video stream URL" << std::endl;
        std::cout << "  --ip        <addr> Specify the command server IP address" << std::endl;
        std::cout << "  --port      <port> Specify the command server port" << std::endl;
    }

    extern "C" int main(int argc, char* argv[]) {
        std::string video_url = "http://10.0.0.10:8080/stream/video.mjpeg";
        std::string ip = "10.0.0.10";
        int port = 12345;

        enum class ArgState { DEFAULT, VIDEO_URL, IP, PORT };
        ArgState state = ArgState::DEFAULT;
        for (int i = 1; i < argc; i ++) {
            auto arg = std::string(argv[i]);

            switch (state) {
                case ArgState::DEFAULT: {
                    if (arg == "--video-url") {
                        state = ArgState::VIDEO_URL;
                    }
                    else if (arg == "--ip") {
                        state = ArgState::IP;
                    }
                    else if (arg == "--port") {
                        state = ArgState::PORT;
                    }
                    else if (arg == "--help") {
                        display_help();
                        return 0;
                    }
                    else {
                        std::cerr << "Invalid argument '" << arg << "'." << std::endl;
                        return 1;
                    }
                    break;
                }

                case ArgState::VIDEO_URL: {
                    video_url = arg;
                    state = ArgState::DEFAULT;
                    break;
                }

                case ArgState::IP: {
                    ip = arg;
                    state = ArgState::DEFAULT;
                    break;
                }

                case ArgState::PORT: {
                    if (std::sscanf(arg.c_str(), "%d", &port) != 1) {
                        std::cerr << "Expected integer after '--port', not '" << arg << "'." << std::endl;
                        return 1;
                    }
                    state = ArgState::DEFAULT;
                    break;
                }

                default: break;
            }
        }

        switch (state) {
            case ArgState::VIDEO_URL: {
                std::cerr << "Expected argument after '--video-url'" << std::endl;
                return 1;
            }

            case ArgState::IP: {
                std::cerr << "Expected argument after '--ip'" << std::endl;
                return 1;
            }

            case ArgState::PORT: {
                std::cerr << "Expected argument after '--port'" << std::endl;
                return 1;
            }

            default: break;
        }

        return run(video_url, ip, port);
    }
}

//// owl.cpp : Defines the entry point for the console application.
///* Phil Culverhouse Oct 2016 (c) Plymouth UNiversity
// *
// * Uses IP sockets to communicate to the owl robot (see owl-comms.h)
// * Uses OpenCV to perform normalised cross correlation to find a match to a template
// * (see owl-cv.h).
// * PWM definitions for the owl servos are held in owl-pwm.h
// * includes bounds check definitions
// * requires setting for specific robot
// *
// * This demonstration programs does the following:
// * a) loop 1 - take picture, check arrow keys
// *             move servos +5 pwm units for each loop
// *             draw 64x64 pixel square overlaid on Right image
// *             if 'c' is pressed copy patch into a template for matching with left
// *              exit loop 1;
// * b) loop 2 - perform Normalised Cross Correlation between template and left image
// *             move Left eye to centre on best match with template
// *             (treats Right eye are dominate in this example).
// *             loop
// *             on exit by ESC key
// *                  go back to loop 1
// *
// * First start communcations on Pi by running 'python PFCpacket.py'
// * Then run this program. The Pi server prints out [Rx Ry Lx Ly] pwm values and loops
// *
// * NOTE: this program is just a demonstrator, the right eye does not track, just the left.
// */

//#include <iostream>
//#include <fstream>

//#include <sys/types.h>
//#include <unistd.h>

//#include "owl-pwm.h"
//#include "owl-comms.h"
//#include "owl-cv.h"


//#include <iostream> // for standard I/O
//#include <string>   // for strings
//#include <mutex>
//#include <thread>
//#include <algorithm>

//using namespace std;
//using namespace cv;

//int sign(int x) { return (x > 0) ? 1 : ((x < 0) ? -1 : 0); }

//struct Targets
//{
//    int eye_l_x = LxC;
//    int eye_l_y = LyC;
//    int eye_r_x = RxC;
//    int eye_r_y = RyC;
//    int neck = NeckC;
//    bool exit = false;

//    void center()
//    {
//        this->eye_l_x = LxC;
//        this->eye_l_y = LyC;
//        this->eye_r_x = RxC;
//        this->eye_r_y = RyC;
//        this->neck = NeckC;
//    }
//};

//std::mutex tgt_lock;
//Targets targets;

//void worker(SOCKET sock)
//{
//    int turn_rate = 10;
//    Targets current;
//    while (true)
//    {
//        tgt_lock.lock();
//        volatile Targets tgts = targets;
//        tgt_lock.unlock();

//        current.eye_l_x += sign(tgts.eye_l_x - current.eye_l_x) * std::min(std::abs(tgts.eye_l_x - current.eye_l_x), turn_rate);
//        current.eye_l_y += sign(tgts.eye_l_y - current.eye_l_y) * std::min(std::abs(tgts.eye_l_y - current.eye_l_y), turn_rate);
//        current.eye_r_x += sign(tgts.eye_r_x - current.eye_r_x) * std::min(std::abs(tgts.eye_r_x - current.eye_r_x), turn_rate);
//        current.eye_r_y += sign(tgts.eye_r_y - current.eye_r_y) * std::min(std::abs(tgts.eye_r_y - current.eye_r_y), turn_rate);
//        current.neck += sign(tgts.neck - current.neck) * std::min(std::abs(tgts.neck - current.neck), turn_rate);

//        ostringstream strm;
//        strm << current.eye_r_x << " " << current.eye_r_y << " " << current.eye_l_x << " " << current.eye_l_y << " " << current.neck;
//        string s = strm.str();
//        OwlSendPacket (sock, s.c_str());

//        if (tgts.exit)
//            break;

//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//    }
//}

//void keythread()
//{
//    while (true)
//    {
//        int key = waitKey(0);

//        tgt_lock.lock();

//        const int turnRate = 100;
//        switch (key){
//        case 'w'://2490368: Changed BILL//up arrow
//            targets.eye_r_y += turnRate;
//            targets.eye_l_y -= turnRate;
//            break;
//        case 's'://2621440: Changed BILL//down arrow
//            targets.eye_r_y -= turnRate;
//            targets.eye_l_y += turnRate;
//            break;
//        case 'a'://2424832: Changed BILL//left arrow
//            targets.eye_r_x -= turnRate;
//            targets.eye_l_x -= turnRate;
//            break;
//        case 'd'://2555904: Changed BILL// right arrow
//            targets.eye_r_x += turnRate;
//            targets.eye_l_x += turnRate;
//            break;
//        case 'q':
//            targets.neck += turnRate;
//            break;
//        case 'e':
//            targets.neck -= turnRate;
//            break;
//        default:
//            break;
//        }

//        bool should_exit = targets.exit;

//        tgt_lock.unlock();

//        if (should_exit)
//            break;
//    }
//}

//std::mutex frame_lock;
//cv::Mat hidden_frame;
//cv::Mat* frame;
//bool frame_valid = false;
//VideoCapture* cap_ptr = nullptr;
//void framethread(string source)
//{
//    while (true)
//    {
//        if (cap_ptr != nullptr)
//        {
//            if (!cap_ptr->read(hidden_frame))
//                cout  << "Could not open the input video: " << source << endl;
//            else
//            {
//                frame_lock.lock();
//                frame_valid = true;
//                frame_lock.unlock();
//            }
//        }

//        frame_lock.lock();
//        frame = &hidden_frame;
//        frame_lock.unlock();

//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//    }
//}

//int main(int argc, char *argv[])
//{
//    char receivedStr[1024];
//    ostringstream CMDstream; // string packet
//    string CMD;
//    int N;

//    Rx = RxLm; Lx = LxLm;
//    Ry = RyC; Ly = LyC;
//    Neck= NeckC;

//    string source ="http://10.0.0.10:8080/stream/video.mjpeg"; // was argv[1];           // the source file name
//    string PiADDR = "10.0.0.10";

//    //SETUP TCP COMMS
//    int PORT=12345;
//    SOCKET u_sock = OwlCommsInit ( PORT, PiADDR);

//    std::thread work_thread(worker, u_sock);
//    //std::thread input_thread(keythread);
//    std::thread frame_thread(framethread, source);

//    /***********************
// * LOOP continuously for testing
// */
//    // RyC=RyC-40; LyC=LyC+40; // offset for cross on card
//    Rx = RxC; Lx = LxC;
//    Ry = RyC; Ly = LyC;
//    Neck= NeckC;

//    const Mat OWLresult;// correlation result passed back from matchtemplate
//    cv::Mat Frame;
//    Mat Left, Right; // images
//    bool inLOOP=true; // run through cursor control first, capture a target then exit loop

//    while (inLOOP){
//        // move servos to centre of field
//        CMDstream.str("");
//        CMDstream.clear();
//        CMDstream << Rx << " " << Ry << " " << Lx << " " << Ly << " " << Neck;
//        CMD = CMDstream.str();
//        string RxPacket= OwlSendPacket (u_sock, CMD.c_str());

//        VideoCapture cap (source);              // Open input
//        if (!cap.isOpened())
//        {
//            cout  << "Could not open the input video: " << source << endl;
//            return -1;
//        }

//        frame_lock.lock();
//        cap_ptr = &cap;
//        frame_lock.unlock();

//        //Rect region_of_interest = Rect(x, y, w, h);
//        while (inLOOP){
////            if (!cap.read(Frame))
////            {
////                cout  << "Could not open the input video: " << source << endl;
////                //         break;
////            }
//            Mat FrameFlpd;

//            while (true)
//            {
//                frame_lock.lock();
//                bool valid = frame_valid;
//                frame_lock.unlock();

//                if (valid)
//                    break;
//            }

//            frame_lock.lock();
//            cv::flip(*frame,FrameFlpd,1); // Note that Left/Right are reversed now
//            frame_lock.unlock();

//            //Mat Gray; cv::cvtColor(Frame, Gray, cv::COLOR_BGR2GRAY);
//            // Split into LEFT and RIGHT images from the stereo pair sent as one MJPEG iamge
//            Left= FrameFlpd( Rect(0, 0, 640, 480)); // using a rectangle
//            Right=FrameFlpd( Rect(640, 0, 640, 480)); // using a rectangle
//            Mat RightCopy;
//            Right.copyTo(RightCopy);
//            rectangle( RightCopy, target, Scalar::all(255), 2, 8, 0 ); // draw white rect
//            imshow("Left",Left);imshow("Right", RightCopy);
//            //waitKey(1); // display the images

//            int key = waitKey(1);

//            tgt_lock.lock();

//            const int turnRate = 10;
//            switch (key){
//            case 'w'://2490368: Changed BILL//up arrow
//                targets.eye_r_y += turnRate;
//                targets.eye_l_y -= turnRate;
//                break;
//            case 's'://2621440: Changed BILL//down arrow
//                targets.eye_r_y -= turnRate;
//                targets.eye_l_y += turnRate;
//                break;
//            case 'a'://2424832: Changed BILL//left arrow
//                targets.eye_r_x -= turnRate;
//                targets.eye_l_x -= turnRate;
//                break;
//            case 'd'://2555904: Changed BILL// right arrow
//                targets.eye_r_x += turnRate;
//                targets.eye_l_x += turnRate;
//                break;
//            case 'q':
//                targets.neck += turnRate;
//                break;
//            case 'e':
//                targets.neck -= turnRate;
//                break;
//            case 'r':
//                targets.center();
//                break;
//            default:
//                break;
//            }

//            targets.eye_l_x = min(LxRm, max(LxLm, targets.eye_l_x));
//            targets.eye_l_y = min(LyBm, max(LyTm, targets.eye_l_y));
//            targets.eye_r_x = min(RxRm, max(RxLm, targets.eye_r_x));
//            targets.eye_r_y = min(RyTm, max(RyBm, targets.eye_r_y));
//            targets.neck = min(NeckL, max(NeckR, targets.neck));

//            tgt_lock.unlock();

////            tgt_lock.lock();
////            targets.eye_l_x = Lx;
////            targets.eye_l_y = Ly;
////            targets.eye_r_x = Rx;
////            targets.eye_r_y = Ry;
////            targets.neck = Neck;
////            tgt_lock.unlock();

////                CMDstream.str("");
////                CMDstream.clear();
////                CMDstream << Rx << " " << Ry << " " << Lx << " " << Ly << " " << Neck;
////                CMD = CMDstream.str();
////                RxPacket= OwlSendPacket (u_sock, CMD.c_str());

////                if (0) {
////                    for (int i=0;i<10;i++){
////                        Rx=Rx-50; Lx=Lx-50;
////                        CMDstream.str("");
////                        CMDstream.clear();
////                        CMDstream << Rx << " " << Ry << " " << Lx << " " << Ly << " " << Neck;
////                        CMD = CMDstream.str();
////                        RxPacket= OwlSendPacket (u_sock, CMD.c_str());
////                        //waitKey(100); // cut the pause for a smooth persuit camera motion
////                    }
////                }
//            } // END cursor control loop
//            // close windows down
//            destroyAllWindows();
//            // just a ZMCC
//            // right is the template, just captured manually
//            inLOOP=true; // run through the loop until decided to exit
//            while (inLOOP) {
//                if (!cap.read(Frame))
//                {
//                    cout  << "Could not open the input video: " << source << endl;
//                    break;
//                }
//                Mat FrameFlpd; cv::flip(Frame,FrameFlpd,1); // Note that Left/Right are reversed now
//                //Mat Gray; cv::cvtColor(Frame, Gray, cv::COLOR_BGR2GRAY);
//                // Split into LEFT and RIGHT images from the stereo pair sent as one MJPEG iamge
//                Left= FrameFlpd( Rect(0, 0, 640, 480)); // using a rectangle
//                Right=FrameFlpd( Rect(640, 0, 640, 480)); // using a rectangle

//                //Rect target= Rect(320-32, 240-32, 64, 64); //defined in owl-cv.h
//                //Mat OWLtempl(Right, target);
//                OwlCorrel OWL;
//                OWL = Owl_matchTemplate( Right,  Left, OWLtempl, target);
//                /// Show me what you got
//                Mat RightCopy;
//                Right.copyTo(RightCopy);
//                rectangle( RightCopy, target, Scalar::all(255), 2, 8, 0 );
//                rectangle( Left, OWL.Match, Point( OWL.Match.x + OWLtempl.cols , OWL.Match.y + OWLtempl.rows), Scalar::all(255), 2, 8, 0 );
//                rectangle( OWLresult, OWL.Match, Point( OWL.Match.x + OWLtempl.cols , OWL.Match.y + OWLtempl.rows), Scalar::all(255), 2, 8, 0 );

//                imshow("Owl-L", Left);
//                imshow("Owl-R", RightCopy);
//                imshow("Correl",OWL.Result );
//                if (waitKey(10)== 27) inLOOP=false;
//// P control
//                double KPx=0.1; // track rate X
//                double KPy=0.1; // track rate Y
//                double LxScaleV = LxRangeV/(double)640; //PWM range /pixel range
//                double Xoff= 320-(OWL.Match.x + OWLtempl.cols)/LxScaleV ; // compare to centre of image
//                int LxOld=Lx;

//                Lx=LxOld-Xoff*KPx; // roughly 300 servo offset = 320 [pixel offset


//                double LyScaleV = LyRangeV/(double)480; //PWM range /pixel range
//                double Yoff= (250+(OWL.Match.y + OWLtempl.rows)/LyScaleV)*KPy ; // compare to centre of image
//                int LyOld=Ly;
//                Ly=LyOld-Yoff; // roughly 300 servo offset = 320 [pixel offset

//                cout << Lx << " " << Xoff << " " << LxOld << endl;
//                cout << Ly << " " << Yoff << " " << LyOld << endl;
//                //Atrous

//                //Maxima

//                // Align cameras

//                // ZMCC disparity map

//                // ACTION

//                // move to get minimise distance from centre of both images, ie verge in to targe
//                // move servos to position
//                CMDstream.str("");
//                CMDstream.clear();
//                CMDstream << Rx << " " << Ry << " " << Lx << " " << Ly << " " << Neck;
//                CMD = CMDstream.str();
//                RxPacket= OwlSendPacket (u_sock, CMD.c_str());


//            } // end if ZMCC
//        } // end while outer loop
//#ifdef __WIN32__
//        closesocket(u_sock);
//#else
//        close(clientSock);
//#endif
//        exit(0); // exit here for servo testing only
//    }


//// OWL eye ranges (max)
//int RyBm = 1120; // (bottom) to
//int RyTm = 2000; //(top)
//int RxRm = 1890; //(right) to
//int RxLm = 1200; //(left)
//int LyBm = 2000; //(bottom) to
//int LyTm = 1180; //(top)
//int LxRm = 1850; // (right) to
//int LxLm = 1180; // (left)
//int NeckR = 1100;
//int NeckL = 1950;
//// VGA match ranges
//int RyBv = 1240; // (bottom) to
//int RyTv = 1655; //(top)
//int RxRv = 1845; //(right) to
//int RxLv = 1245; //(left)
//int LyBv = 1880; //(bottom) to
//int LyTv = 1420; //(top)
//int LxRv = 1835; // (right) to
//int LxLv = 1265; // (left)
//int RxC=1545;
//int RyC=1460;
//int LxC=1545;
//int LyC=1560;
//int NeckC = 1530;
//int Ry,Rx,Ly,Lx,Neck; // calculate values for position
////MAX servo eye socket ranges
//int RyRangeM=RyTm-RyBm;
//int RxRangeM=RxRm-RxLm;
//int LyRangeM=LyTm-LyBm; // reflected so negative
//int LxRangeM=LxRm-LxLm;
//int NeckRange=NeckL-NeckR;
////vga CAMERA ranges
//int RyRangeV=RyTv-RyBv;
//int RxRangeV=RxRv-RxLv;
//int LyRangeV=LyTv-LyBv; // reflected so negative
//int LxRangeV=LxRv-LxLv;
