using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO.Ports;
using Microsoft.Win32;
using System.Threading;
using System.Windows.Threading;
using System.Text.RegularExpressions;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel;

namespace HapticMouse
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        static MenuItem mnuPorts;

        private readonly RoutedEventHandler choosePort = new RoutedEventHandler(Port_Choose);
        private DispatcherTimer timer = new DispatcherTimer();
        public static string comPortName = "";
        private string filename = "data.csv";
        private int[] vals;
        private readonly BackgroundWorker worker = new BackgroundWorker();
        private SerialPort comPort = new SerialPort();
        private bool stop = false;
        private bool blocked = false;
        private string extention = ".csv";

        public MainWindow()
        {
            InitializeComponent();
            mnuPorts = this.mnuPort;
            timer.Tick += Timer_tick;
            timer.Interval = new TimeSpan(0, 0, 5);
            timer.Start();
            //string directory = System.AppDomain.CurrentDomain.BaseDirectory;
            //string fullFileName = System.IO.Path.Combine(directory, "AppData", this.filename);
            //System.IO.FileInfo fi = new System.IO.FileInfo(fullFileName);
            //if (fi.Exists)
            //{
            //    try
            //    {
            //        List<int> readVals = new List<int>();
            //        string [] lines = System.IO.File.ReadAllLines(fi.FullName).ToArray();
            //        foreach (string line in lines)
            //        {
            //            string [] sepVals = line.Split(",");
            //            foreach (string val in sepVals)
            //            {
            //                readVals.Add((int)double.Parse(val));
            //            }
            //        }
            //        this.vals = readVals.ToArray();
            //        this.txtOutput.Text += string.Format("Files was read from: {0} \n", fi.FullName);
            //    }
            //    catch (System.IO.IOException exp)
            //    {
            //        MessageBox.Show(exp.Message, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            //    }
            //}
            //else {
            //    MessageBox.Show("File not exist: " + fi.FullName);
            //}
            worker.DoWork += worker_DoWork;
            worker.RunWorkerCompleted += worker_RunWorkerCompleted;
            worker.WorkerSupportsCancellation = true;
            this.LoadList();
        }

        public static void Port_Choose(object Sender, System.EventArgs e) {
            for(int position = 0; position < mnuPorts.Items.Count; position++)
            {
                (mnuPorts.Items[position] as MenuItem).IsChecked = false;
            }
            (Sender as MenuItem).IsChecked = true;
            comPortName = (Sender as MenuItem).Header.ToString();
        }

        public void Timer_tick(object Sender, EventArgs e) {
            string[] ports = SerialPort.GetPortNames();
            Array.Reverse(ports);
            this.update_entries(ports);
        }

        private void update_entries(string[] portNames)
        {
            // Remove not existing ports
            List<string> availablePorts = new List<string>();
            List<int> unavailablePorts = new List<int>();
            int position = 0;
            foreach (MenuItem menuItem in this.mnuPort.Items)
            {
                if (portNames.Contains(menuItem.Header.ToString()))
                {
                    availablePorts.Add(menuItem.Header.ToString());
                }
                else
                {
                    unavailablePorts.Add(position);
                }
                position++;
            }
            unavailablePorts.Reverse();
            foreach (int index in unavailablePorts)
            {
                if (comPortName.Equals((this.mnuPort.Items[index] as MenuItem).Header.ToString()))
                {
                    comPortName = "";
                }
                this.mnuPort.Items.RemoveAt(index);
            }
            // Add new ports
            foreach (string portName in portNames)
            {
                if (!availablePorts.Contains(portName))
                {
                    MenuItem newPort = new MenuItem();
                    newPort.Click += choosePort;
                    newPort.Header = portName;
                    this.mnuPort.Items.Add(newPort);
                }
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            timer.Stop();
        }

        private void btnSend_Click(object sender, RoutedEventArgs e)
        {
            if (comPortName.Equals(""))
            {
                MessageBox.Show("Choose port");
            }
            else
            {
                if (!is_valid_frequency(txtFrequency.Text))
                {
                    MessageBox.Show("Frequency must be digits only");
                    return;
                }
                if (comPort.IsOpen)
                {
                    comPort.Close();
                }
                comPort.PortName = comPortName;
                comPort.BaudRate = int.Parse(this.txtBaudRate.Text);
                comPort.Open();
                this.txtOutput.Text += string.Format("Com port: {0} is connected\n", comPortName);
                stop = false;
                worker.RunWorkerAsync("Press Enter in the next 5 seconds to Cancel operation:");
                blocked = false;
                this.btnSend.IsEnabled = false;
                this.btnStop.IsEnabled = true;
                this.btnBlock.IsEnabled = true;
            }
        }

        public void send_frequency(string voltage)
        {
            //$2,100,0,300,0#
            string message = String.Format(@"$1,{0}#", voltage);
            comPort.Write(message);
            //this.sendAsyncTextToOutput("Succesfully send: -----" + message + "----- \n");
        }

        private bool is_valid_frequency(string freq)
        {
            bool valid = true;
            Regex nonDigits = new Regex(@"\D");
            MatchCollection matches = nonDigits.Matches(freq);
            valid &= (matches.Count == 0) & (freq != "");
            return valid;
        }

        private void txt_NumbersOnly(object sender, TextCompositionEventArgs e)
        {
            Regex regex = new Regex("[^0-9]+");
            e.Handled = regex.IsMatch(e.Text);
        }


        private void worker_DoWork(object sender, DoWorkEventArgs e)
        {
            this.sendAsyncTextToOutput("Start sending data \n");
            while (!stop)
            {
                for (int index = 0; index < this.vals.Count(); index++)
                {
                    if (stop) break;
                    int freg = (int)this.vals[index];
                    send_frequency(freg.ToString());
                }
            }
            send_frequency("0");
        }

        private void worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            this.sendAsyncTextToOutput("Port closed \n");
            comPort.Close();
        }

        private void btnStop_Click(object sender, RoutedEventArgs e)
        {
            //this.worker.CancelAsync();
            stop = true;
            this.btnSend.IsEnabled = true;
            this.btnStop.IsEnabled = false;
            this.btnBlock.IsEnabled = false;
            //comPort.Close();
        }

        private void sendAsyncTextToOutput(string message)
        {
            this.Dispatcher.Invoke(() =>
            {
                txtOutput.Text += message;
            });
        }

        private void btnBlock_Click(object sender, RoutedEventArgs e)
        {
            if (!this.blocked)
            {
                stop = true;
                System.Threading.Thread.Sleep(500);
                this.btnSend.IsEnabled = false;
                this.txtBlockBtn.Text = "Unblock";
                if (!comPort.IsOpen)
                {
                    comPort.Open();
                }
                this.send_frequency("100");
                comPort.Close();
            }
            else
            {
                stop = false;
                System.Threading.Thread.Sleep(500);
                comPort.Open();
                worker.RunWorkerAsync("Press Enter in the next 5 seconds to Cancel operation:");
                this.btnSend.IsEnabled = true;
                this.txtBlockBtn.Text = "Block";
            }
            blocked = !blocked;
        }
        private void LoadList()
        {
            this.cmbTextures.Items.Clear();
            this.cmbTextures.Items.Add("data");
            this.cmbTextures.Items.Add("data_hard_cover");
            this.cmbTextures.SelectedIndex = 0;
        }

        private void LoadTexture(string filename)
        {
            string directory = System.AppDomain.CurrentDomain.BaseDirectory;
            string fullFileName = System.IO.Path.Combine(directory, "AppData", filename);
            System.IO.FileInfo fi = new System.IO.FileInfo(fullFileName);
            if (fi.Exists)
            {
                try
                {
                    List<int> readVals = new List<int>();
                    string[] lines = System.IO.File.ReadAllLines(fi.FullName).ToArray();
                    foreach (string line in lines)
                    {
                        string[] sepVals = line.Split(",");
                        foreach (string val in sepVals)
                        {
                            readVals.Add((int)double.Parse(val));
                        }
                    }
                    this.vals = readVals.ToArray();
                    this.txtOutput.Text += string.Format("Files was read from: {0} \n", fi.FullName);
                }
                catch (System.IO.IOException exp)
                {
                    MessageBox.Show(exp.Message, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            else
            {
                MessageBox.Show("File not exist: " + fi.FullName);
            }
        }

        private void cmbTextures_Change(object sender, SelectionChangedEventArgs e)
        {
            //MessageBox.Show(this.cmbTextures.SelectedItem.ToString());
            this.LoadTexture(this.cmbTextures.SelectedItem.ToString() + extention);
        }
    }
}
