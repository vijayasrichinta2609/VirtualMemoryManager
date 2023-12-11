#include <iostream>
#include <unordered_map>
#include <queue>
class TLBEntry {
public:
    int page_number;
    int frame_number;
    TLBEntry() : page_number(-1), frame_number(-1) {}
    TLBEntry(int page, int frame) : page_number(page), frame_number(frame) {}
};
class PageTable {
private:
    std::unordered_map<int, int> entries;
public:
    PageTable() {}
    int get_frame_number(int page_number) {
        if (entries.find(page_number) != entries.end()) {
            return entries[page_number];
        } else {
            return -1;
        }
    }
    void insert_page(int page_number, int frame_number) {
        entries[page_number] = frame_number;
    }
    void display_page_table() {
        std::cout << "Page Table:" << std::endl;
        std::cout << "Page Number | Frame Number" << std::endl;
        for (const auto& entry : entries) {
            std::cout << entry.first << "\t\t" << entry.second << std::endl;
        }
        std::cout << std::endl;
    }
};
class VirtualMemoryManager {
private:
    PageTable page_table;
    std::unordered_map<int, TLBEntry> tlb;
    std::queue<int> page_fifo;
    int tlb_capacity;
    int tlb_hits;
    int tlb_misses;
    int page_table_hits;
    int page_table_misses;
    int total_accesses;
public:
    VirtualMemoryManager(int tlb_size) : tlb_capacity(tlb_size), tlb_hits(0), tlb_misses(0),
                                          page_table_hits(0), page_table_misses(0), total_accesses(0) {}
    void update_tlb(int page, int frame) {
        TLBEntry entry(page, frame);
        if (tlb.size() >= tlb_capacity) {
            int oldest_page = page_fifo.front();
            page_fifo.pop();
            tlb.erase(oldest_page);
        }
        tlb[page] = entry;
        page_fifo.push(page);
    }
    void translate_virtual_to_physical(int virtual_address) {
        total_accesses++;
        int page_number = virtual_address / 4096;
        int offset = virtual_address % 4096;
        if (tlb.find(page_number) != tlb.end()) {
            TLBEntry tlb_entry = tlb[page_number];
            int frame_number = tlb_entry.frame_number;
            int physical_address = frame_number * 4096 + offset;
            std::cout << "Virtual Address: " << virtual_address << ", TLB hit, Page Hit. Physical Address: " << physical_address << std::endl;
            tlb_hits++;
        } else {
            std::cout << "Virtual Address: " << virtual_address << ", TLB miss, ";
            int frame_number = page_table.get_frame_number(page_number);
            if (frame_number != -1) {
                int physical_address = frame_number * 4096 + offset;
                std::cout << "Page Table Hit. Physical Address: " << physical_address << std::endl;
                tlb_misses++;
                page_table_hits++;
                update_tlb(page_number, frame_number);
            } else {
                std::cout << "Page Table Miss (Page Fault).";
                tlb_misses++;
                page_table_misses++;
                int new_frame_number = page_number + 4;
                page_table.insert_page(page_number, new_frame_number);
                update_tlb(page_number, new_frame_number);
                int physical_address = new_frame_number * 4096 + offset;
                std::cout << " Physical Address: " << physical_address << std::endl;
            }
        }
        page_table.display_page_table();
        print_tlb_table();
    }
    void print_tlb_table() {
        std::cout << "TLB Table:" << std::endl;
        std::cout << "Entry | Page Number | Frame Number" << std::endl;
        for (const auto& entry : tlb) {
            std::cout << entry.first << "\t\t" << entry.second.page_number << "\t\t" << entry.second.frame_number << std::endl;
        }
        std::cout << std::endl;
    }
    void print_metrics() {
        std::cout << "TLB Hits: " << tlb_hits << std::endl;
        std::cout << "TLB Misses: " << tlb_misses << std::endl;
        std::cout << "Page Table Hits: " << page_table_hits << std::endl;
        std::cout << "Page Table Misses: " << page_table_misses << std::endl;
        // Calculate ratios
        double tlb_hit_ratio_value = static_cast<double>(tlb_hits) / total_accesses;
        double tlb_miss_ratio = static_cast<double>(tlb_misses) / total_accesses;
        double page_table_hit_ratio = static_cast<double>(page_table_hits) / total_accesses;
        double page_table_miss_ratio = static_cast<double>(page_table_misses) / total_accesses;
        std::cout << "TLB Hit Ratio: " << tlb_hit_ratio_value << std::endl;
        std::cout << "TLB Miss Ratio: " << tlb_miss_ratio << std::endl;
        std::cout << "Page Table Hit Ratio: " << page_table_hit_ratio << std::endl;
        std::cout << "Page Table Miss Ratio: " << page_table_miss_ratio << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        // Calculate EMAT
        double emat = tlb_hit_ratio_value * (10 + 100) + tlb_miss_ratio * (10 + 2 * 100);
        std::cout << "Effective Memory Access Time (EMAT): " << emat << " ns" << std::endl;
    }
};
int main() {
    VirtualMemoryManager vmm(2);
    vmm.translate_virtual_to_physical(0);
    vmm.translate_virtual_to_physical(4096);
    vmm.translate_virtual_to_physical(8192);
    vmm.translate_virtual_to_physical(12288);
    vmm.translate_virtual_to_physical(4096);
    vmm.translate_virtual_to_physical(16384);
    vmm.translate_virtual_to_physical(16384);
    vmm.print_metrics();
    return 0;
}


