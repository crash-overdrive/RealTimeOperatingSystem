int main() {
    asm volatile("ldr pc, [pc, #24]");
}