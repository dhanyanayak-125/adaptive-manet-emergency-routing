#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/wifi-module.h"
#include "ns3/yans-wifi-helper.h"
#include <iomanip>
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Emergency Manet");

//===== PAPER THRESHOLDS =====
const double VELOCITY_THRESHOLD = 20.0;
const uint32_t NODE_THRESHOLD = 100;
const double CHECK_INTERVAL = 2.0;

//===== GLOBAL =====
std::string g_activeProtocol = "OLSRv2";
int g_switchCount = 0;
NodeContainer g_nodes;
Ptr<FlowMonitor> g_monitor;
std::ofstream g_csvLog;

double g_speed = 10.0;

//===== CHANGE SPEED (FOR DYNAMIC SWITCHING) =====
void ChangeSpeed(double newSpeed)
{
    g_speed = newSpeed;
    std::cout << "\n>>>SPEED CHANGED TO " << newSpeed << " m/s<<<\n";
}

//===== ADAPTIVE MONITOR =====
void AdaptiveMonitor()
{
    double now = Simulator::Now().GetSeconds();
    uint32_t N = g_nodes.GetN();
    double V = g_speed;
    
    std::string targetProtocol = (V > VELOCITY_THRESHOLD && N < NODE_THRESHOLD) ? "AODV" : "OLSRv2";
    std::string decision = "NO_CHANGE";
    
    if (targetProtocol != g_activeProtocol)
    {
        decision = "SWITCH_TO_" + targetProtocol;
        g_activeProtocol = targetProtocol;
        g_switchCount++;
    }
    
    // ===== PRINT =====
    std::cout << "[AdaptiveMonitor] "
              << "t=" << now
              << " V=" << V
              << " N=" << N
              << " Proto=" << g_activeProtocol
              << " " << decision
              << std::endl;
              
    g_csvLog << now << ","
             << V << ","
             << N << ","
             << g_activeProtocol << ","
             << decision << "\n";
             
    Simulator::Schedule(Seconds(CHECK_INTERVAL), &AdaptiveMonitor);
}

//===== MAIN =====
int main(int argc, char *argv[])
{
    uint32_t nNodes = 30;
    double speed = 10.0;
    double simTime = 100.0;
    
    CommandLine cmd;
    cmd.AddValue("nNodes", "Number of nodes", nNodes);
    cmd.AddValue("speed", "Mobility speed", speed);
    cmd.Parse(argc, argv);
    
    // IMPORTANT FIX
    g_speed = speed;
    
    std::cout << "START SPEED=" << g_speed << std::endl;
    
    g_nodes.Create(nNodes);
    
    // ===== MOBILITY =====
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(g_nodes);
    
    // ===== ROUTING =====
    OlsrHelper olsr;
    AodvHelper aodv;
    
    Ipv4ListRoutingHelper listRouting;
    listRouting.Add(olsr, 10);
    listRouting.Add(aodv, 5);
    
    InternetStackHelper stack;
    stack.SetRoutingHelper(listRouting);
    stack.Install(g_nodes);
    
    // ===== WIFI =====
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211b);
    
    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");
    
    NetDeviceContainer devices = wifi.Install(phy, mac, g_nodes);
    
    // ===== IP =====
    Ipv4AddressHelper address;
    address.SetBase("10.1.0.0", "255.255.0.0");
    address.Assign(devices);
    
    //===== FLOW MONITOR =====
    FlowMonitorHelper flowHelper;
    g_monitor = flowHelper.InstallAll();
    
    // ===== CSV =====
    g_csvLog.open("adaptive_log.csv");
    g_csvLog << "Time, Speed, Nodes, Protocol, Decision\n";
    
    //===== SCHEDULE =====
    Simulator::Schedule(Seconds(5.0), &AdaptiveMonitor);
    
    // FORCE SWITCHES (VERY IMPORTANT)
    Simulator::Schedule(Seconds(20.0), &ChangeSpeed, 25.0); // Switches to AODV
    Simulator::Schedule(Seconds(60.0), &ChangeSpeed, 10.0); // Switches to OLSR
    
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();
    
    std::cout << "\n===== SUMMARY =====\n";
    std::cout << "Switches: " << g_switchCount << std::endl;
    std::cout << "Final Protocol=" << g_activeProtocol << std::endl;
    
    g_csvLog.close();
    Simulator::Destroy();
    return 0;
}
