#include "coro_for_graph_v2.hpp"
#include <iostream>
#include <chrono>
#include <vector>


thread_local tcalloc coroutine_allocator;


inline void prefetch(const void* ptr) 
{
    typedef struct { char x[CACHE_LINE_SIZE]; } cacheline_t;
    asm volatile("prefetcht0 %0" : : "m"(*(const cacheline_t*)ptr));
  //__builtin_prefetch(*(const cacheline_t*)ptr));
}


struct alignas(CACHE_LINE_SIZE) Node 
{
    int64_t value_array[7];
    Node* next;

    Node() : next(nullptr) 
    {
        for(int i = 0; i < 6; ++i)
            this->value_array[i] = rand() % 100 + 1;
    }

    int64_t complex_computation()
    {
        int64_t result = 0;
        for(int i = 0; i < 6; ++i)
        {
            if(i % 4 == 0) result += this->value_array[i];
            else if(i % 4 == 1) result -= this->value_array[i];
            else if(i * 4 == 1) result *= this->value_array[i];
            else if(i / 4 == 1) result /= this->value_array[i];
        }
        return result;
    } 
};


Node* node_list = new Node[10000];


class LinkedList
{
public:

    Node* head;

    template<typename... Args>
    LinkedList(Args... args)
    {
        head = nullptr;
        Node* current = nullptr;
        add_nodes(head, current, args...);
    }

    template<typename T, typename... Args>
    void add_nodes(Node*& head, Node*& current, T first, Args... rest) {
        Node* new_node = &node_list[first];
        if(!head)
        {
            head = new_node;
            current = head;
        } 
        else 
        {
            current->next = new_node;
            current = current->next;
        }
        if(sizeof...(rest) > 0)
        {
            add_nodes(head, current, rest...);
        }
    }

    void add_nodes(Node*& head, Node*& current) {}

    void print_myself()
    {
        auto current = head;
        while(current != nullptr)
        {
            std::cout << current->complex_computation() << " ";
            current = current->next;
        }
    }
};


class Timer 
{
public:

    void start() 
    {
        start_time = std::chrono::high_resolution_clock::now();
        last_lap_time = start_time;
        running = true;
        std::cout << "Timer started.\n";
    }

    void lap(std::string func_name) 
    {
        if (!running) 
        {
            std::cout << "Timer has not started yet.\n";
            return;
        }
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> lap_duration = now - last_lap_time;
        last_lap_time = now;
        std::cout << func_name << " run time: " << lap_duration.count() << " us.\n";
    }

    void end() 
    {
        if (!running) 
        {
            std::cout << "Timer has not started yet.\n";
            return;
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> total_duration = end_time - start_time;
        running = false;
        std::cout << "Timer stopped. Total time: " << total_duration.count() << " us.\n";
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point last_lap_time;
    bool running = false;
};


std::vector<int> get_user_input(const int node_num)
{
    std::vector<int> values(node_num);
    std::cout << "Enter " << node_num << " values: ";
    for (int i = 0; i < node_num; ++i) {
        std::cin >> values[i];
    }
    return values;
}


generator<void> coro_print_linked_list(const LinkedList& ll)
{
    auto tmp_node = ll.head;
    while(tmp_node != nullptr)
    {
        prefetch(tmp_node);
        co_await std::suspend_always{};
        std::cout << tmp_node->complex_computation() << " ";
        tmp_node = tmp_node->next;
    }
}

template <typename T, std::size_t... Is>
auto init_linked_list_from_vector(const std::vector<T>& vec, std::index_sequence<Is...>) 
{
    return LinkedList(vec[Is]...);
}

int main()
{
    constexpr int node_num = 3;
    Timer clk;
    clk.start();

    auto linked_list_parameter_1 = get_user_input(node_num);
    clk.lap("input 1");

    auto linked_list_parameter_2 = get_user_input(node_num);
    clk.lap("input 2");

    LinkedList linked_list_1 = init_linked_list_from_vector(
        linked_list_parameter_1, 
        std::make_index_sequence<node_num>{}
    );
    clk.lap("init linked list 1");

    LinkedList linked_list_2 = init_linked_list_from_vector(
        linked_list_parameter_2,
        std::make_index_sequence<node_num>{}
    );
    clk.lap("init linked list 2");

    int mode = 0;
    std::cin >> mode;
    clk.lap("input mode");

    // sequential
    if(mode == 0)
    {
        linked_list_1.print_myself();
        linked_list_2.print_myself();
        clk.lap("compute & print");
    }
    // interleaved (coro)
    else
    {
        std::vector<std::coroutine_handle<>> compute_tasks(2);
        compute_tasks[0] = coro_print_linked_list(linked_list_1).get_handle();
        compute_tasks[1] = coro_print_linked_list(linked_list_2).get_handle();
        clk.lap("coro init");

        int finished = 0;
        while(finished < compute_tasks.size())
        {
            for(auto &task : compute_tasks)
            {
                if(task)
                {
                    if(task.done())
                    {
                        finished++;
                        task.destroy();
                        task = nullptr;
                    }
                    else
                    {
                        task.resume();
                    }
                }
            }
        }
        clk.lap("compute & print");
    }

    clk.end();
}