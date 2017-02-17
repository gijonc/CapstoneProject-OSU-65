using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApplication2
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            label9.Text = "0";
            aGauge3.Value = 0;
            label10.Text = "0";
            aGauge2.Value = 0;
            label8.Text = "0";
            aGauge1.Value = 0;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            trackBar1.Minimum = -90;
            trackBar1.Maximum = 90;
            trackBar1.TickStyle = TickStyle.BottomRight;
            trackBar1.TickFrequency = 1;
            trackBar3.Minimum = 0;
            trackBar3.Maximum = 360;
            trackBar3.TickStyle = TickStyle.BottomRight;
            trackBar3.TickFrequency = 1;
            trackBar2.Minimum = -90;
            trackBar2.Maximum = 90;
            trackBar2.TickStyle = TickStyle.BottomRight;
            trackBar2.TickFrequency = 1;
        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            label9.Text = trackBar1.Value.ToString();
            aGauge3.Value = trackBar1.Value;
            aGauge3.Refresh();
        }

        private void trackBar2_Scroll(object sender, EventArgs e)
        {
            label8.Text = trackBar2.Value.ToString();
            aGauge1.Value = trackBar2.Value;
            aGauge1.Refresh();
        }

        private void trackBar3_Scroll(object sender, EventArgs e)
        {
            label10.Text = trackBar3.Value.ToString();
            aGauge2.Value = trackBar3.Value;
            aGauge2.Refresh();
        }
    }
}
