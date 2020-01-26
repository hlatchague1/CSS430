import java.util.*;

class Shell extends Thread{
    public void run(){
        int lineCounter = 1;
        while(true){
            SysLib.cout("Shell[" + lineCounter + "]% ");
            StringBuffer buffer = new StringBuffer();
            SysLib.cin(buffer);
            String[] commandLine = SysLib.stringToArgs(buffer.toString());
            int argLength = commandLine.length;

            //converts arguments into String
            String fullCommandLine = buffer.toString();

            //checks for empty command
            if(fullCommandLine.isEmpty()){
                continue;
            }
            //checks if user wants to exit program
            if(commandLine[0].equals("exit")){
                break;
            }
            
            //separates full command line at semicolon or ampersand
            String[] semicolonSplitLine = fullCommandLine.split(";");
            String[] ampersantSplitLine = fullCommandLine.split("&");

            int semiCommandLength = semicolonSplitLine.length;
            int ampCommandLength = ampersantSplitLine.length;
            
            //if & and ; are present
            if(semiCommandLength > 1 && ampCommandLength > 1){
                ampAndSemiExec(fullCommandLine);
            }

            //if only ; is present
            else if(semiCommandLength > 1){
                semiExec(semicolonSplitLine);
            }

            //if only & is present
            else{
                ampExec(ampersantSplitLine);
            }
            //increments n in "Shell[n]%"
            lineCounter++;
        }
        SysLib.cout("Done!");
        SysLib.sync();
        SysLib.exit();
    }

    //deals with commands that include & and ; 
    public void ampAndSemiExec(String commandLine) {
        String[] semicolonSplit = commandLine.split(";");
        int semiLength = semicolonSplit.length;
		for (int i = 0; i < semiLength; i++) {
            String[] ampersandSplit = semicolonSplit[i].split("&");
            int ampLength = ampersandSplit.length;
			if (ampLength == 1) {
                String[] commandLine2 = SysLib.stringToArgs(semicolonSplit[i]);
                int tid = SysLib.exec(commandLine2);
                SysLib.cout("Started process " + commandLine2[0] + " with thread tid = " + tid + ":\n");
                SysLib.join();
			}
			else {
				ampExec(ampersandSplit);
			}
		}
    }

    //deals with commands including only &
    //processes are running at the same time
	public void ampExec(String[] commandLine) {
        int counter = 0;
        int length = commandLine.length;
		for (int i = 0; i < length; i++) {
            String[] commandLineToString = SysLib.stringToArgs(commandLine[i]);
            int tid = SysLib.exec(commandLineToString);
            SysLib.cout("Started process " + commandLineToString[0] + " with thread tid = " + tid + ":\n");
            counter++;
		}
		for (int j = 0; j < counter; j++) {
			SysLib.join();
        }
    }

       //deals with commands including only ;
       //processes run one after the other
       public void semiExec(String[] commandLine) {
        int length = commandLine.length;
		for (int i = 0; i < length; i++) {
			String[] commandLineToString = SysLib.stringToArgs(commandLine[i]);
            int tid = SysLib.exec(commandLineToString);            
            SysLib.cout("Started process " + commandLineToString[0] + " with thread tid = " + tid + ":\n");
            SysLib.join();
        }
	}
}
