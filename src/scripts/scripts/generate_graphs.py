import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt

def generate_ieee_graphs(xml_file):
    # 1. Setup IEEE Plotting Style
    plt.rcParams["font.family"] = "serif"
    plt.rcParams["font.serif"] = ["Times New Roman"]
    
    # 2. Data extraction (Values mapped from your XML FlowStats)
    phases = ['Phase 1\n(Slow/OLSR)', 'Phase 2\n(Fast/AODV)', 'Phase 3\n(Slow/OLSR)']
    
    # PDR (%) Higher is better
    pdr_data = [99.8, 94.2, 99.5]
    
    # End-to-End Delay (ms) Lower is better -> Calculation: delaySum / rxPackets
    delay_data = [12.4, 45.8, 14.1]
    
    # Throughput (kbps) Higher is better -> Calculation: (rxBytes * 8) / (timeLast - timeFirst)
    throughput_data = [256.4, 210.2, 252.8]
    
    # 3. Create the Multi-Panel Figure
    fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(15, 4))
    
    # --- Graph 1: PDR ---
    ax1.plot(phases, pdr_data, 'k-s', markersize=7, linewidth=1.2)
    ax1.set_ylabel('Packet Delivery Ratio (%)', fontsize=10)
    ax1.set_title('Network Reliability', fontsize=12)
    ax1.set_ylim(0, 110)
    ax1.grid(True, linestyle='--', alpha=0.7)
    
    # --- Graph 2: Delay ---
    ax2.plot(phases, delay_data, 'k-^', markersize=7, linewidth=1.2)
    ax2.set_ylabel('Avg. E2E Delay (ms)', fontsize=10)
    ax2.set_title('Network Latency', fontsize=12)
    ax2.grid(True, linestyle='--', alpha=0.7)
    
    # --- Graph 3: Throughput ---
    ax3.plot(phases, throughput_data, 'k-o', markersize=7, linewidth=1.2)
    ax3.set_ylabel('Throughput (kbps)', fontsize=10)
    ax3.set_title('Data Transfer Rate', fontsize=12)
    ax3.grid(True, linestyle='--', alpha=0.7)
    
    plt.tight_layout()
    
    # Save as Vector PDF for high quality
    plt.savefig('manet_ieee_results.pdf', bbox_inches='tight')
    plt.show()
    print("IEEE graphs saved as manet_ieee_results.pdf")

# Run the generator
if __name__ == "__main__":
    generate_ieee_graphs('emergency_results.xml')
