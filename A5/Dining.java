//
// Simple Java implementation of the classic Dining Philosophers problem.
//
// No synchronization (yet).
//
// Graphics are *very* naive.  Philosophers are big blobs.
// Forks are little blobs.
// 
// Written by Michael Scott, 1997; updated 2013 to use Swing.
// Updated again in 2019 to drop support for applets.
//

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import java.util.*;
import java.lang.*;
import java.lang.Thread.*;

// This code has six main classes:
//  Dining
//      The public, "main" class.
//  Philosopher
//      Active -- extends Thread
//  Fork
//      Passive
//  Table
//      Manages the philosophers and forks and their physical layout.
//  Coordinator
//      Provides mechanisms to suspend, resume, and reset the state of
//      worker threads (philosophers).
//  UI
//      Manages graphical layout and button presses.

public class Dining {
    private static final int CANVAS_SIZE = 360;
        // pixels in each direction;
        // needs to agree with size in dining.html

    public static void main(String[] args) {
        JFrame f = new JFrame("Dining");
        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        Dining me = new Dining();

        final Coordinator c = new Coordinator();
        final Table t = new Table(c, CANVAS_SIZE);
        // arrange to call graphical setup from GUI thread
        try {
            SwingUtilities.invokeAndWait(new Runnable() {
                public void run() {
                    new UI(f, c, t);
                }
            });
        } catch (Exception e) {
            System.err.println("unable to create GUI");
        }

        f.pack();            // calculate size of frame
        f.setVisible(true);
    }
}

class Fork {
    private Table t;
    private static final int XSIZE = 10;
    private static final int YSIZE = 10;
    private int orig_x;
    private int orig_y;
    private int x;
    private int y;

    // Constructor.
    // cx and cy indicate coordinates of center.
    // Note that fillOval method expects coordinates of upper left corner
    // of bounding box instead.
    //
    public Fork(Table T, int cx, int cy) {
        t = T;
        orig_x = cx;
        orig_y = cy;
        x = cx;
        y = cy;
    }

    public void reset() {
        clear();
        x = orig_x;
        y = orig_y;
        t.repaint();
    }

    // arguments are coordinates of acquiring philosopher's center
    //
    public void acquire(int px, int py) {
        clear();
        x = (orig_x + px)/2;
        y = (orig_y + py)/2;
        t.repaint();
    }

    public void release() {
        reset();
    }

    // render self
    //
    public void draw(Graphics g) {
        g.setColor(Color.black);
        g.fillOval(x-XSIZE/2, y-YSIZE/2, XSIZE, YSIZE);
    }
    // erase self
    //
    private void clear() {
        Graphics g = t.getGraphics();
        g.setColor(t.getBackground());
        g.fillOval(x-XSIZE/2, y-YSIZE/2, XSIZE, YSIZE);
    }
}

class Philosopher extends Thread {
    private static final Color THINK_COLOR = Color.blue;
    private static final Color WAIT_COLOR = Color.red;
    private static final Color EAT_COLOR = Color.green;
    private static final double THINK_TIME = 4.0;
    private static final double FUMBLE_TIME = 2.0;
        // time between becoming hungry and grabbing first fork
    private static final double EAT_TIME = 3.0;

    private Coordinator c;
    private Table t;
    private static final int XSIZE = 50;
    private static final int YSIZE = 50;
    private int x;
    private int y;
    private Fork left_fork;
    private Fork right_fork;
    private Random prn;
    private Color color;

    // Constructor.
    // cx and cy indicate coordinates of center
    // Note that fillOval method expects coordinates of upper left corner
    // of bounding box instead.
    //
    public Philosopher(Table T, int cx, int cy,
                       Fork lf, Fork rf, Coordinator C) {
        t = T;
        x = cx;
        y = cy;
        left_fork = lf;
        right_fork = rf;
        c = C;
        prn = new Random();
        color = THINK_COLOR;
    }

    // start method of Thread calls run; you don't
    //
    public void run() {
        for (;;) {
            try {
                if (c.gate()) delay(EAT_TIME/2.0);
                think();
                if (c.gate()) delay(THINK_TIME/2.0);
                hunger();
                if (c.gate()) delay(FUMBLE_TIME/2.0);
                eat();
            } catch(ResetException e) { 
                color = THINK_COLOR;
                t.repaint();
            }
        }
    }

    // render self
    //
    public void draw(Graphics g) {
        g.setColor(color);
        g.fillOval(x-XSIZE/2, y-YSIZE/2, XSIZE, YSIZE);
    }

    // sleep for secs +- FUDGE (%) seconds
    //
    private static final double FUDGE = 0.2;
    private void delay(double secs) throws ResetException {
        double ms = 1000 * secs;
        int window = (int) (2.0 * ms * FUDGE);
        int add_in = prn.nextInt() % window;
        int original_duration = (int) ((1.0-FUDGE) * ms + add_in);
        int duration = original_duration;
        for (;;) {
            try {
                Thread.sleep(duration);
                return;
            } catch(InterruptedException e) {
                if (c.isReset()) {
                    throw new ResetException();
                } else {        // suspended
                    c.gate();   // wait until resumed
                    duration = original_duration / 2;
                    // don't wake up instantly; sleep for about half
                    // as long as originally instructed
                }
            }
        }
    }

    private void think() throws ResetException {
        color = THINK_COLOR;
        t.repaint();
        delay(THINK_TIME);
    }

    private void hunger() throws ResetException {
        color = WAIT_COLOR;
        t.repaint();
        delay(FUMBLE_TIME);
        left_fork.acquire(x, y);
        yield();    // you aren't allowed to remove this
        right_fork.acquire(x, y);
    }

    private void eat() throws ResetException {
        color = EAT_COLOR;
        t.repaint();
        delay(EAT_TIME);
        left_fork.release();
        yield();    // you aren't allowed to remove this
        right_fork.release();
    }
}

// Graphics panel in which philosophers and forks appear.
//
class Table extends JPanel {
    private static final int NUM_PHILS = 5;

    // following fields are set by construcctor:
    private final Coordinator c;
    private Fork[] forks;
    private Philosopher[] philosophers;

    public void pause() {
        c.pause();
        // force philosophers to notice change in coordinator state:
        for (int i = 0; i < NUM_PHILS; i++) {
            philosophers[i].interrupt();
        }
    }

    // Called by the UI when it wants to start over.
    //
    public void reset() {
        c.reset();
        // force philosophers to notice change in coordinator state:
        for (int i = 0; i < NUM_PHILS; i++) {
            philosophers[i].interrupt();
        }
        for (int i = 0; i < NUM_PHILS; i++) {
            forks[i].reset();
        }
    }

    // The following method is called automatically by the graphics
    // system when it thinks the Table canvas needs to be re-displayed.
    // This can happen because code elsewhere in this program called
    // repaint(), or because of hiding/revealing or open/close
    // operations in the surrounding window system.
    //
    public void paintComponent(Graphics g) {
        super.paintComponent(g);

        for (int i = 0; i < NUM_PHILS; i++) {
            forks[i].draw(g);
            philosophers[i].draw(g);
        }
        g.setColor(Color.black);
        g.drawRect(0, 0, getWidth()-1, getHeight()-1);
    }

    // Constructor
    //
    // Note that angles are measured in radians, not degrees.
    // The origin is the upper left corner of the frame.
    //
    public Table(Coordinator C, int CANVAS_SIZE) {    // constructor
        c = C;
        forks = new Fork[NUM_PHILS];
        philosophers = new Philosopher[NUM_PHILS];
        setPreferredSize(new Dimension(CANVAS_SIZE, CANVAS_SIZE));

        for (int i = 0; i < NUM_PHILS; i++) {
            double angle = Math.PI/2 + 2*Math.PI/NUM_PHILS*(i-0.5);
            forks[i] = new Fork(this,
                (int) (CANVAS_SIZE/2.0 + CANVAS_SIZE/6.0 * Math.cos(angle)),
                (int) (CANVAS_SIZE/2.0 - CANVAS_SIZE/6.0 * Math.sin(angle)));
        }
        for (int i = 0; i < NUM_PHILS; i++) {
            double angle = Math.PI/2 + 2*Math.PI/NUM_PHILS*i;
            philosophers[i] = new Philosopher(this,
                (int) (CANVAS_SIZE/2.0 + CANVAS_SIZE/3.0 * Math.cos(angle)),
                (int) (CANVAS_SIZE/2.0 - CANVAS_SIZE/3.0 * Math.sin(angle)),
                forks[i],
                forks[(i+1) % NUM_PHILS],
                c);
            philosophers[i].start();
        }
    }
}

class ResetException extends Exception { };

// The Coordinator serves to slow down execution, so that behavior is
// visible on the screen, and to notify all running threads when the user
// wants them to reset.
//
class Coordinator {
    public enum State { PAUSED, RUNNING, RESET }
    private State state = State.PAUSED;

    public synchronized boolean isPaused() {
        return (state == State.PAUSED);
    }

    public synchronized void pause() {
        state = State.PAUSED;
    }

    public synchronized boolean isReset() {
        return (state == State.RESET);
    }

    public synchronized void reset() {
        state = State.RESET;
    }

    public synchronized void resume() {
        state = State.RUNNING;
        notifyAll();        // wake up all waiting threads
    }

    // Return true if we were forced to wait because the coordinator was
    // paused or reset.
    //
    public synchronized boolean gate() throws ResetException {
        if (state == State.PAUSED || state == State.RESET) {
            try {
                wait();
            } catch(InterruptedException e) {
                if (isReset()) {
                    throw new ResetException();
                }
            }
            return true;        // waited
        }
        return false;           // didn't wait
    }
}

// Class UI is the user interface.  It displays a Table canvas above
// a row of buttons.  Actions (event handlers) are defined for each of
// the buttons.  Depending on the state of the UI, either the "run" or
// the "pause" button is the default (highlighted in most window
// systems); it will often self-push if you hit carriage return.
//
class UI extends JPanel {
    private final Coordinator c;
    private final Table t;

    private final JRootPane root;
    private static final int externalBorder = 6;

    private static final int stopped = 0;
    private static final int running = 1;
    private static final int paused = 2;

    private int state = stopped;

    // Constructor
    //
    public UI(RootPaneContainer pane, Coordinator C, Table T) {
        final UI u = this;
        c = C;
        t = T;

        final JPanel b = new JPanel();   // button panel

        final JButton runButton = new JButton("Run");
        final JButton pauseButton = new JButton("Pause");
        final JButton resetButton = new JButton("Reset");
        final JButton quitButton = new JButton("Quit");

        runButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                c.resume();
                root.setDefaultButton(pauseButton);
            }
        });
        pauseButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                t.pause();
                root.setDefaultButton(runButton);
            }
        });
        resetButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                t.reset();
                root.setDefaultButton(runButton);
            }
        });
        quitButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                System.exit(0);
            }
        });

        // put the buttons into the button panel:
        b.setLayout(new FlowLayout());
        b.add(runButton);
        b.add(pauseButton);
        b.add(resetButton);
        b.add(quitButton);

        // put the Table canvas and the button panel into the UI:
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        setBorder(BorderFactory.createEmptyBorder(
            externalBorder, externalBorder, externalBorder, externalBorder));
        add(t);
        add(b);

        // put the UI into the Frame
        pane.getContentPane().add(this);
        root = getRootPane();
        root.setDefaultButton(runButton);
    }
}
