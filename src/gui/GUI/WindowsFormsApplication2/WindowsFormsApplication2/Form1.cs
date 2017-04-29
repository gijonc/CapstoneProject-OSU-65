using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MathNet.Numerics.Statistics;
using MathNet.Numerics.Distributions;
using MathNet.Numerics.LinearAlgebra.Double;

namespace WindowsFormsApplication2
{
    public partial class Form1 : Form
    {
        List<float> yawArray;
        List<float> pitchArray;
        List<float> rollArray;

        public Form1()
        {
            InitializeComponent();
            serialPort1.Open();
            label9.Text = "0";
            aGauge3.Value = 0;
            label10.Text = "0";
            aGauge2.Value = 0;
            label8.Text = "0";
            aGauge1.Value = 0;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            timer1.Interval = 2000;
            timer1.Start();
        }


        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            String data = serialPort1.ReadLine().ToString();
            String[] dataArray = data.Split(',');
            float yaw = float.Parse(dataArray[0]);
            float pitch = float.Parse(dataArray[1]);
            float roll = float.Parse(dataArray[2]);
            SetOriginalData(data);
            SetText8(dataArray[1]);
            SetText9(dataArray[2]);
            SetText10(dataArray[0]);
            SetGauge1(pitch);
            SetGauge2(yaw);
            SetGauge3(roll);
        }

        delegate void SetTextCallback(string text);
        delegate void SetFloatCallback(float value);

        private void SetGauge1(float value)
        {
            if (this.aGauge1.InvokeRequired)
            {
                SetFloatCallback d = new SetFloatCallback(SetGauge1);
                this.Invoke(d, new object[] { value });
            }
            else
            {
                this.aGauge1.Value = value;
                aGauge1.Refresh();
            }
        }

        private void SetGauge2(float value)
        {
            if (this.aGauge2.InvokeRequired)
            {
                SetFloatCallback d = new SetFloatCallback(SetGauge2);
                this.Invoke(d, new object[] { value });
            }
            else
            {
                if (value >= 0)
                {
                    this.aGauge2.Value = value;
                    aGauge2.Refresh();
                }
                else
                {
                    value = 360 - value;
                    this.aGauge2.Value = value;
                    aGauge2.Refresh();
                }
            }
        }

        private void SetGauge3(float value)
        {
            if (this.aGauge3.InvokeRequired)
            {
                SetFloatCallback d = new SetFloatCallback(SetGauge3);
                this.Invoke(d, new object[] { value });
            }
            else
            {
                this.aGauge3.Value = value;
                aGauge3.Refresh();
            }
        }

        private void SetText8(string text)
        {
            if (this.label8.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText8);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.label8.Text = text;
            }
        }

        private void SetText9(string text)
        {
            if (this.label9.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText9);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.label9.Text = text;
            }
        }

        private void SetText10(string text)
        {
            if (this.label10.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText10);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.label10.Text = text;
            }
        }

        private void SetOriginalData(string text)
        {
            if (this.label12.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetOriginalData);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.label12.Text = text;
            }
        }
        private double mean(double[] sample)
        {
            double average = sample.Mean();
            return average;
        }
        private double confidence95(double[] sample)
        {
            double sampleSize = sample.Count();
            double sd = sample.StandardDeviation();
            double interval = 1.96 * (sd / Math.Sqrt(sampleSize));
            return interval;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {   
            // For debugging 
            // MessageBox.Show("This shows every 2 seconds");
        }
    }
}
