// InstaChrome Java-like System Configuration
package instachrome.system;

public class InstaChromeOS {
    public static void main(String[] args) {
        System.out.println("InstaChromeOS v1.0");
        System.out.println("Java Runtime Environment");
        System.out.println("Starting system services...");
        
        // Initialize core services
        initMemory();
        initFS();
        initNetwork();
        
        // Start shell
        Shell shell = new Shell();
        shell.run();
    }
    
    private static void initMemory() {
        Memory memory = Memory.getInstance();
        memory.displayInfo();
    }
    
    private static void initFS() {
        FileSystem fs = new FAT32FileSystem();
        fs.mount("/");
    }
    
    private static void initNetwork() {
        NetworkStack net = NetworkStack.getInstance();
        net.initialize();
    }
}

class Shell {
    public void run() {
        System.out.println("\n========================================");
        System.out.println("         welcome to InstaChromeOS");
        System.out.println("========================================");
        System.out.println("                  Powerd by InstaChrome");
        
        java.util.Scanner scanner = new java.util.Scanner(System.in);
        String input;
        
        while(true) {
            System.out.print("\nroot> ");
            input = scanner.nextLine();
            
            switch(input) {
                case "dir":
                    printDir();
                    break;
                case "lst":
                    listCurrent();
                    break;
                case "help":
                    printHelp();
                    break;
                case "clear":
                    System.out.print("\033[H\033[2J");
                    break;
                default:
                    if(input.startsWith("printf * ")) {
                        System.out.println(input.substring(9));
                    } else if(!input.isEmpty()) {
                        System.out.println("Command not found");
                    }
            }
        }
    }
    
    private void printDir() {
        System.out.println("\nRoot/");
        System.out.println("+-------InstaChromeOS");
        System.out.println("+                      +---------system/");
        System.out.println("+                      +                     +----int_handler/");
        System.out.println("+                      +                     +                 +----int.ih");
        System.out.println("+                      +                     +---- OSsys.os");
        System.out.println("+                      +                     +---- Linker.hmk");
        System.out.println("+                      +                     +---- instachrome.jl");
        System.out.println("+                      +------ personal/");
        System.out.println("+                      +------ space/");
        System.out.println("+                      +------ tmp/");
    }
    
    private void listCurrent() {
        System.out.println("\nCurrent directory contents:");
        System.out.println("system/");
        System.out.println("personal/");
        System.out.println("space/");
        System.out.println("tmp/");
    }
    
    private void printHelp() {
        System.out.println("\nAvailable commands:");
        System.out.println("  dir              - Display full filesystem");
        System.out.println("  lst              - List current directory");
        System.out.println("  cd <dir>         - Change directory");
        System.out.println("  tayn <file>      - Create new file");
        System.out.println("  mkdir <dir>      - Create new directory");
        System.out.println("  printf * <text>  - Print text");
        System.out.println("  clear            - Clear screen");
        System.out.println("  help             - Show this help");
    }
}