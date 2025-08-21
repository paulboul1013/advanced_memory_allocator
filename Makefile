CXX := g++
CXXFLAGS := -Wall -Wextra  -O2 

# Debug 模式標誌
DEBUG_FLAGS := -D_DEBUG -g

# 目標檔名
TARGET := main

# 所有的 .cpp 檔
SRCS := main.cpp LinearAllocator.cpp Allocator.cpp Benchmark.cpp CAllocator.cpp free_tree_allocator.cpp

# 自動生成對應的 .o 檔
OBJS := $(SRCS:.cpp=.o)

# 自動生成的依賴檔 (.d)
DEPS := $(OBJS:.o=.d)

# 規則：編譯並連結
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 編譯 .cpp 成 .o，並自動生成 .d
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug 模式編譯
.PHONY: debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)

# 清理
.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

# 包含自動生成的依賴檔
-include $(DEPS)
