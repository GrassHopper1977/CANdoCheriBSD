
// This file has been generated by the GUI designer. Do not modify.
public partial class MainWindow
{
	private global::Gtk.UIManager UIManager;
	private global::Gtk.Action FileAction;
	private global::Gtk.Action HelpAction;
	private global::Gtk.Action ExitAction;
	private global::Gtk.Action Action;
	private global::Gtk.Action ViewAction;
	private global::Gtk.Action ClearAction;
	private global::Gtk.VBox vbox1;
	private global::Gtk.Table table2;
	private global::Gtk.Button button6;
	private global::Gtk.Button button7;
	private global::Gtk.Button button8;
	private global::Gtk.ScrolledWindow GtkScrolledWindow;
	private global::Gtk.TextView textview1;
	private global::Gtk.Frame frame1;
	private global::Gtk.Alignment GtkAlignment;
	private global::Gtk.Table table1;
	private global::Gtk.Button button1;
	private global::Gtk.CheckButton checkbutton1;
	private global::Gtk.Entry entry1;
	private global::Gtk.Entry entry2;
	private global::Gtk.Entry entry3;
	private global::Gtk.Entry entry4;
	private global::Gtk.Entry entry5;
	private global::Gtk.Entry entry6;
	private global::Gtk.Entry entry7;
	private global::Gtk.Entry entry8;
	private global::Gtk.Entry entry9;
	private global::Gtk.Label label1;
	private global::Gtk.Label label10;
	private global::Gtk.Label label11;
	private global::Gtk.Label label12;
	private global::Gtk.Label label2;
	private global::Gtk.Label label3;
	private global::Gtk.Label label4;
	private global::Gtk.Label label5;
	private global::Gtk.Label label6;
	private global::Gtk.Label label7;
	private global::Gtk.Label label8;
	private global::Gtk.Label label9;
	private global::Gtk.RadioButton radiobutton1;
	private global::Gtk.RadioButton radiobutton2;
	private global::Gtk.SpinButton spinbutton1;
	private global::Gtk.Label GtkLabel7;
	private global::Gtk.Statusbar statusbar2;

	protected virtual void Build ()
	{
		global::Stetic.Gui.Initialize (this);
		// Widget MainWindow
		this.UIManager = new global::Gtk.UIManager ();
		global::Gtk.ActionGroup w1 = new global::Gtk.ActionGroup ("Default");
		this.FileAction = new global::Gtk.Action ("FileAction", global::Mono.Unix.Catalog.GetString ("File"), null, null);
		this.FileAction.ShortLabel = global::Mono.Unix.Catalog.GetString ("File");
		w1.Add (this.FileAction, null);
		this.HelpAction = new global::Gtk.Action ("HelpAction", global::Mono.Unix.Catalog.GetString ("Help"), null, null);
		this.HelpAction.ShortLabel = global::Mono.Unix.Catalog.GetString ("Help");
		w1.Add (this.HelpAction, null);
		this.ExitAction = new global::Gtk.Action ("ExitAction", global::Mono.Unix.Catalog.GetString ("Exit"), null, null);
		this.ExitAction.ShortLabel = global::Mono.Unix.Catalog.GetString ("Exit");
		w1.Add (this.ExitAction, null);
		this.Action = new global::Gtk.Action ("Action", null, null, null);
		w1.Add (this.Action, null);
		this.ViewAction = new global::Gtk.Action ("ViewAction", global::Mono.Unix.Catalog.GetString ("View"), null, null);
		this.ViewAction.ShortLabel = global::Mono.Unix.Catalog.GetString ("View");
		w1.Add (this.ViewAction, null);
		this.ClearAction = new global::Gtk.Action ("ClearAction", global::Mono.Unix.Catalog.GetString ("Clear"), null, null);
		this.ClearAction.ShortLabel = global::Mono.Unix.Catalog.GetString ("Clear");
		w1.Add (this.ClearAction, null);
		this.UIManager.InsertActionGroup (w1, 0);
		this.AddAccelGroup (this.UIManager.AccelGroup);
		this.Name = "MainWindow";
		this.Title = global::Mono.Unix.Catalog.GetString ("MonoDevelop libCANdo.so Example - GTK - v1.0");
		this.WindowPosition = ((global::Gtk.WindowPosition)(1));
		// Container child MainWindow.Gtk.Container+ContainerChild
		this.vbox1 = new global::Gtk.VBox ();
		this.vbox1.Name = "vbox1";
		this.vbox1.Spacing = 12;
		this.vbox1.BorderWidth = ((uint)(12));
		// Container child vbox1.Gtk.Box+BoxChild
		this.table2 = new global::Gtk.Table (((uint)(1)), ((uint)(3)), false);
		this.table2.Name = "table2";
		this.table2.RowSpacing = ((uint)(6));
		this.table2.ColumnSpacing = ((uint)(6));
		// Container child table2.Gtk.Table+TableChild
		this.button6 = new global::Gtk.Button ();
		this.button6.CanFocus = true;
		this.button6.Name = "button6";
		this.button6.UseUnderline = true;
		this.button6.Label = global::Mono.Unix.Catalog.GetString ("Start");
		this.table2.Add (this.button6);
		global::Gtk.Table.TableChild w2 = ((global::Gtk.Table.TableChild)(this.table2 [this.button6]));
		w2.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table2.Gtk.Table+TableChild
		this.button7 = new global::Gtk.Button ();
		this.button7.CanFocus = true;
		this.button7.Name = "button7";
		this.button7.UseUnderline = true;
		this.button7.Label = global::Mono.Unix.Catalog.GetString ("Stop");
		this.table2.Add (this.button7);
		global::Gtk.Table.TableChild w3 = ((global::Gtk.Table.TableChild)(this.table2 [this.button7]));
		w3.LeftAttach = ((uint)(1));
		w3.RightAttach = ((uint)(2));
		w3.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table2.Gtk.Table+TableChild
		this.button8 = new global::Gtk.Button ();
		this.button8.CanFocus = true;
		this.button8.Name = "button8";
		this.button8.UseUnderline = true;
		this.button8.Label = global::Mono.Unix.Catalog.GetString ("Exit");
		this.table2.Add (this.button8);
		global::Gtk.Table.TableChild w4 = ((global::Gtk.Table.TableChild)(this.table2 [this.button8]));
		w4.LeftAttach = ((uint)(2));
		w4.RightAttach = ((uint)(3));
		w4.YOptions = ((global::Gtk.AttachOptions)(4));
		this.vbox1.Add (this.table2);
		global::Gtk.Box.BoxChild w5 = ((global::Gtk.Box.BoxChild)(this.vbox1 [this.table2]));
		w5.Position = 0;
		w5.Expand = false;
		w5.Fill = false;
		// Container child vbox1.Gtk.Box+BoxChild
		this.GtkScrolledWindow = new global::Gtk.ScrolledWindow ();
		this.GtkScrolledWindow.Name = "GtkScrolledWindow";
		this.GtkScrolledWindow.ShadowType = ((global::Gtk.ShadowType)(1));
		// Container child GtkScrolledWindow.Gtk.Container+ContainerChild
		this.textview1 = new global::Gtk.TextView ();
		this.textview1.CanFocus = true;
		this.textview1.Name = "textview1";
		this.textview1.Editable = false;
		this.textview1.CursorVisible = false;
		this.textview1.AcceptsTab = false;
		this.GtkScrolledWindow.Add (this.textview1);
		this.vbox1.Add (this.GtkScrolledWindow);
		global::Gtk.Box.BoxChild w7 = ((global::Gtk.Box.BoxChild)(this.vbox1 [this.GtkScrolledWindow]));
		w7.Position = 1;
		// Container child vbox1.Gtk.Box+BoxChild
		this.frame1 = new global::Gtk.Frame ();
		this.frame1.Name = "frame1";
		this.frame1.ShadowType = ((global::Gtk.ShadowType)(0));
		// Container child frame1.Gtk.Container+ContainerChild
		this.GtkAlignment = new global::Gtk.Alignment (0F, 0F, 1F, 1F);
		this.GtkAlignment.Name = "GtkAlignment";
		this.GtkAlignment.LeftPadding = ((uint)(12));
		// Container child GtkAlignment.Gtk.Container+ContainerChild
		this.table1 = new global::Gtk.Table (((uint)(3)), ((uint)(12)), false);
		this.table1.Name = "table1";
		this.table1.RowSpacing = ((uint)(6));
		this.table1.ColumnSpacing = ((uint)(6));
		// Container child table1.Gtk.Table+TableChild
		this.button1 = new global::Gtk.Button ();
		this.button1.Sensitive = false;
		this.button1.CanFocus = true;
		this.button1.Name = "button1";
		this.button1.UseUnderline = true;
		this.button1.Label = global::Mono.Unix.Catalog.GetString ("Now");
		this.table1.Add (this.button1);
		global::Gtk.Table.TableChild w8 = ((global::Gtk.Table.TableChild)(this.table1 [this.button1]));
		w8.TopAttach = ((uint)(1));
		w8.BottomAttach = ((uint)(2));
		w8.LeftAttach = ((uint)(11));
		w8.RightAttach = ((uint)(12));
		w8.XOptions = ((global::Gtk.AttachOptions)(4));
		w8.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.checkbutton1 = new global::Gtk.CheckButton ();
		this.checkbutton1.CanFocus = true;
		this.checkbutton1.Name = "checkbutton1";
		this.checkbutton1.Label = global::Mono.Unix.Catalog.GetString ("29 Bit");
		this.checkbutton1.DrawIndicator = true;
		this.checkbutton1.UseUnderline = true;
		this.table1.Add (this.checkbutton1);
		global::Gtk.Table.TableChild w9 = ((global::Gtk.Table.TableChild)(this.table1 [this.checkbutton1]));
		w9.TopAttach = ((uint)(2));
		w9.BottomAttach = ((uint)(3));
		w9.XOptions = ((global::Gtk.AttachOptions)(4));
		w9.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.entry1 = new global::Gtk.Entry ();
		this.entry1.CanFocus = true;
		this.entry1.Name = "entry1";
		this.entry1.Text = global::Mono.Unix.Catalog.GetString ("0");
		this.entry1.IsEditable = true;
		this.entry1.InvisibleChar = '•';
		this.table1.Add (this.entry1);
		global::Gtk.Table.TableChild w10 = ((global::Gtk.Table.TableChild)(this.table1 [this.entry1]));
		w10.TopAttach = ((uint)(1));
		w10.BottomAttach = ((uint)(2));
		w10.XOptions = ((global::Gtk.AttachOptions)(4));
		w10.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.entry2 = new global::Gtk.Entry ();
		this.entry2.CanFocus = true;
		this.entry2.Name = "entry2";
		this.entry2.Text = global::Mono.Unix.Catalog.GetString ("00");
		this.entry2.IsEditable = true;
		this.entry2.WidthChars = 2;
		this.entry2.MaxLength = 2;
		this.entry2.InvisibleChar = '•';
		this.table1.Add (this.entry2);
		global::Gtk.Table.TableChild w11 = ((global::Gtk.Table.TableChild)(this.table1 [this.entry2]));
		w11.TopAttach = ((uint)(1));
		w11.BottomAttach = ((uint)(2));
		w11.LeftAttach = ((uint)(3));
		w11.RightAttach = ((uint)(4));
		w11.XOptions = ((global::Gtk.AttachOptions)(4));
		w11.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.entry3 = new global::Gtk.Entry ();
		this.entry3.CanFocus = true;
		this.entry3.Name = "entry3";
		this.entry3.Text = global::Mono.Unix.Catalog.GetString ("00");
		this.entry3.IsEditable = true;
		this.entry3.WidthChars = 2;
		this.entry3.MaxLength = 2;
		this.entry3.InvisibleChar = '•';
		this.table1.Add (this.entry3);
		global::Gtk.Table.TableChild w12 = ((global::Gtk.Table.TableChild)(this.table1 [this.entry3]));
		w12.TopAttach = ((uint)(1));
		w12.BottomAttach = ((uint)(2));
		w12.LeftAttach = ((uint)(4));
		w12.RightAttach = ((uint)(5));
		w12.XOptions = ((global::Gtk.AttachOptions)(4));
		w12.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.entry4 = new global::Gtk.Entry ();
		this.entry4.CanFocus = true;
		this.entry4.Name = "entry4";
		this.entry4.Text = global::Mono.Unix.Catalog.GetString ("00");
		this.entry4.IsEditable = true;
		this.entry4.WidthChars = 2;
		this.entry4.MaxLength = 2;
		this.entry4.InvisibleChar = '•';
		this.table1.Add (this.entry4);
		global::Gtk.Table.TableChild w13 = ((global::Gtk.Table.TableChild)(this.table1 [this.entry4]));
		w13.TopAttach = ((uint)(1));
		w13.BottomAttach = ((uint)(2));
		w13.LeftAttach = ((uint)(5));
		w13.RightAttach = ((uint)(6));
		w13.XOptions = ((global::Gtk.AttachOptions)(4));
		w13.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.entry5 = new global::Gtk.Entry ();
		this.entry5.CanFocus = true;
		this.entry5.Name = "entry5";
		this.entry5.Text = global::Mono.Unix.Catalog.GetString ("00");
		this.entry5.IsEditable = true;
		this.entry5.WidthChars = 2;
		this.entry5.MaxLength = 2;
		this.entry5.InvisibleChar = '•';
		this.table1.Add (this.entry5);
		global::Gtk.Table.TableChild w14 = ((global::Gtk.Table.TableChild)(this.table1 [this.entry5]));
		w14.TopAttach = ((uint)(1));
		w14.BottomAttach = ((uint)(2));
		w14.LeftAttach = ((uint)(6));
		w14.RightAttach = ((uint)(7));
		w14.XOptions = ((global::Gtk.AttachOptions)(4));
		w14.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.entry6 = new global::Gtk.Entry ();
		this.entry6.CanFocus = true;
		this.entry6.Name = "entry6";
		this.entry6.Text = global::Mono.Unix.Catalog.GetString ("00");
		this.entry6.IsEditable = true;
		this.entry6.WidthChars = 2;
		this.entry6.MaxLength = 2;
		this.entry6.InvisibleChar = '•';
		this.table1.Add (this.entry6);
		global::Gtk.Table.TableChild w15 = ((global::Gtk.Table.TableChild)(this.table1 [this.entry6]));
		w15.TopAttach = ((uint)(1));
		w15.BottomAttach = ((uint)(2));
		w15.LeftAttach = ((uint)(7));
		w15.RightAttach = ((uint)(8));
		w15.XOptions = ((global::Gtk.AttachOptions)(4));
		w15.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.entry7 = new global::Gtk.Entry ();
		this.entry7.CanFocus = true;
		this.entry7.Name = "entry7";
		this.entry7.Text = global::Mono.Unix.Catalog.GetString ("00");
		this.entry7.IsEditable = true;
		this.entry7.WidthChars = 2;
		this.entry7.MaxLength = 2;
		this.entry7.InvisibleChar = '•';
		this.table1.Add (this.entry7);
		global::Gtk.Table.TableChild w16 = ((global::Gtk.Table.TableChild)(this.table1 [this.entry7]));
		w16.TopAttach = ((uint)(1));
		w16.BottomAttach = ((uint)(2));
		w16.LeftAttach = ((uint)(8));
		w16.RightAttach = ((uint)(9));
		w16.XOptions = ((global::Gtk.AttachOptions)(4));
		w16.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.entry8 = new global::Gtk.Entry ();
		this.entry8.CanFocus = true;
		this.entry8.Name = "entry8";
		this.entry8.Text = global::Mono.Unix.Catalog.GetString ("00");
		this.entry8.IsEditable = true;
		this.entry8.WidthChars = 2;
		this.entry8.MaxLength = 2;
		this.entry8.InvisibleChar = '•';
		this.table1.Add (this.entry8);
		global::Gtk.Table.TableChild w17 = ((global::Gtk.Table.TableChild)(this.table1 [this.entry8]));
		w17.TopAttach = ((uint)(1));
		w17.BottomAttach = ((uint)(2));
		w17.LeftAttach = ((uint)(9));
		w17.RightAttach = ((uint)(10));
		w17.XOptions = ((global::Gtk.AttachOptions)(4));
		w17.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.entry9 = new global::Gtk.Entry ();
		this.entry9.CanFocus = true;
		this.entry9.Name = "entry9";
		this.entry9.Text = global::Mono.Unix.Catalog.GetString ("00");
		this.entry9.IsEditable = true;
		this.entry9.WidthChars = 2;
		this.entry9.MaxLength = 2;
		this.entry9.InvisibleChar = '•';
		this.table1.Add (this.entry9);
		global::Gtk.Table.TableChild w18 = ((global::Gtk.Table.TableChild)(this.table1 [this.entry9]));
		w18.TopAttach = ((uint)(1));
		w18.BottomAttach = ((uint)(2));
		w18.LeftAttach = ((uint)(10));
		w18.RightAttach = ((uint)(11));
		w18.XOptions = ((global::Gtk.AttachOptions)(4));
		w18.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.label1 = new global::Gtk.Label ();
		this.label1.Name = "label1";
		this.label1.LabelProp = global::Mono.Unix.Catalog.GetString ("ID");
		this.table1.Add (this.label1);
		global::Gtk.Table.TableChild w19 = ((global::Gtk.Table.TableChild)(this.table1 [this.label1]));
		w19.XOptions = ((global::Gtk.AttachOptions)(4));
		w19.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.label10 = new global::Gtk.Label ();
		this.label10.Name = "label10";
		this.label10.LabelProp = global::Mono.Unix.Catalog.GetString ("D7");
		this.table1.Add (this.label10);
		global::Gtk.Table.TableChild w20 = ((global::Gtk.Table.TableChild)(this.table1 [this.label10]));
		w20.LeftAttach = ((uint)(9));
		w20.RightAttach = ((uint)(10));
		w20.XOptions = ((global::Gtk.AttachOptions)(4));
		w20.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.label11 = new global::Gtk.Label ();
		this.label11.Name = "label11";
		this.label11.LabelProp = global::Mono.Unix.Catalog.GetString ("D8");
		this.table1.Add (this.label11);
		global::Gtk.Table.TableChild w21 = ((global::Gtk.Table.TableChild)(this.table1 [this.label11]));
		w21.LeftAttach = ((uint)(10));
		w21.RightAttach = ((uint)(11));
		w21.XOptions = ((global::Gtk.AttachOptions)(4));
		w21.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.label12 = new global::Gtk.Label ();
		this.label12.Name = "label12";
		this.table1.Add (this.label12);
		global::Gtk.Table.TableChild w22 = ((global::Gtk.Table.TableChild)(this.table1 [this.label12]));
		w22.LeftAttach = ((uint)(11));
		w22.RightAttach = ((uint)(12));
		w22.XOptions = ((global::Gtk.AttachOptions)(4));
		w22.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.label2 = new global::Gtk.Label ();
		this.label2.Name = "label2";
		this.label2.LabelProp = global::Mono.Unix.Catalog.GetString ("Frame");
		this.table1.Add (this.label2);
		global::Gtk.Table.TableChild w23 = ((global::Gtk.Table.TableChild)(this.table1 [this.label2]));
		w23.LeftAttach = ((uint)(1));
		w23.RightAttach = ((uint)(2));
		w23.XOptions = ((global::Gtk.AttachOptions)(4));
		w23.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.label3 = new global::Gtk.Label ();
		this.label3.Name = "label3";
		this.label3.LabelProp = global::Mono.Unix.Catalog.GetString ("DLC");
		this.table1.Add (this.label3);
		global::Gtk.Table.TableChild w24 = ((global::Gtk.Table.TableChild)(this.table1 [this.label3]));
		w24.LeftAttach = ((uint)(2));
		w24.RightAttach = ((uint)(3));
		w24.XOptions = ((global::Gtk.AttachOptions)(4));
		w24.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.label4 = new global::Gtk.Label ();
		this.label4.Name = "label4";
		this.label4.LabelProp = global::Mono.Unix.Catalog.GetString ("D1");
		this.table1.Add (this.label4);
		global::Gtk.Table.TableChild w25 = ((global::Gtk.Table.TableChild)(this.table1 [this.label4]));
		w25.LeftAttach = ((uint)(3));
		w25.RightAttach = ((uint)(4));
		w25.XOptions = ((global::Gtk.AttachOptions)(4));
		w25.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.label5 = new global::Gtk.Label ();
		this.label5.Name = "label5";
		this.label5.LabelProp = global::Mono.Unix.Catalog.GetString ("D2");
		this.table1.Add (this.label5);
		global::Gtk.Table.TableChild w26 = ((global::Gtk.Table.TableChild)(this.table1 [this.label5]));
		w26.LeftAttach = ((uint)(4));
		w26.RightAttach = ((uint)(5));
		w26.XOptions = ((global::Gtk.AttachOptions)(4));
		w26.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.label6 = new global::Gtk.Label ();
		this.label6.Name = "label6";
		this.label6.LabelProp = global::Mono.Unix.Catalog.GetString ("D3");
		this.table1.Add (this.label6);
		global::Gtk.Table.TableChild w27 = ((global::Gtk.Table.TableChild)(this.table1 [this.label6]));
		w27.LeftAttach = ((uint)(5));
		w27.RightAttach = ((uint)(6));
		w27.XOptions = ((global::Gtk.AttachOptions)(4));
		w27.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.label7 = new global::Gtk.Label ();
		this.label7.Name = "label7";
		this.label7.LabelProp = global::Mono.Unix.Catalog.GetString ("D4");
		this.table1.Add (this.label7);
		global::Gtk.Table.TableChild w28 = ((global::Gtk.Table.TableChild)(this.table1 [this.label7]));
		w28.LeftAttach = ((uint)(6));
		w28.RightAttach = ((uint)(7));
		w28.XOptions = ((global::Gtk.AttachOptions)(4));
		w28.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.label8 = new global::Gtk.Label ();
		this.label8.Name = "label8";
		this.label8.LabelProp = global::Mono.Unix.Catalog.GetString ("D5");
		this.table1.Add (this.label8);
		global::Gtk.Table.TableChild w29 = ((global::Gtk.Table.TableChild)(this.table1 [this.label8]));
		w29.LeftAttach = ((uint)(7));
		w29.RightAttach = ((uint)(8));
		w29.XOptions = ((global::Gtk.AttachOptions)(4));
		w29.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.label9 = new global::Gtk.Label ();
		this.label9.Name = "label9";
		this.label9.LabelProp = global::Mono.Unix.Catalog.GetString ("D6");
		this.table1.Add (this.label9);
		global::Gtk.Table.TableChild w30 = ((global::Gtk.Table.TableChild)(this.table1 [this.label9]));
		w30.LeftAttach = ((uint)(8));
		w30.RightAttach = ((uint)(9));
		w30.XOptions = ((global::Gtk.AttachOptions)(4));
		w30.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.radiobutton1 = new global::Gtk.RadioButton (global::Mono.Unix.Catalog.GetString ("Data"));
		this.radiobutton1.CanFocus = true;
		this.radiobutton1.Name = "radiobutton1";
		this.radiobutton1.DrawIndicator = true;
		this.radiobutton1.UseUnderline = true;
		this.radiobutton1.Group = new global::GLib.SList (global::System.IntPtr.Zero);
		this.table1.Add (this.radiobutton1);
		global::Gtk.Table.TableChild w31 = ((global::Gtk.Table.TableChild)(this.table1 [this.radiobutton1]));
		w31.TopAttach = ((uint)(1));
		w31.BottomAttach = ((uint)(2));
		w31.LeftAttach = ((uint)(1));
		w31.RightAttach = ((uint)(2));
		w31.XOptions = ((global::Gtk.AttachOptions)(4));
		w31.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.radiobutton2 = new global::Gtk.RadioButton (global::Mono.Unix.Catalog.GetString ("Remote"));
		this.radiobutton2.CanFocus = true;
		this.radiobutton2.Name = "radiobutton2";
		this.radiobutton2.DrawIndicator = true;
		this.radiobutton2.UseUnderline = true;
		this.radiobutton2.Group = this.radiobutton1.Group;
		this.table1.Add (this.radiobutton2);
		global::Gtk.Table.TableChild w32 = ((global::Gtk.Table.TableChild)(this.table1 [this.radiobutton2]));
		w32.TopAttach = ((uint)(2));
		w32.BottomAttach = ((uint)(3));
		w32.LeftAttach = ((uint)(1));
		w32.RightAttach = ((uint)(2));
		w32.XOptions = ((global::Gtk.AttachOptions)(4));
		w32.YOptions = ((global::Gtk.AttachOptions)(4));
		// Container child table1.Gtk.Table+TableChild
		this.spinbutton1 = new global::Gtk.SpinButton (1, 8, 1);
		this.spinbutton1.CanFocus = true;
		this.spinbutton1.Name = "spinbutton1";
		this.spinbutton1.Adjustment.PageIncrement = 1;
		this.spinbutton1.ClimbRate = 1;
		this.spinbutton1.Numeric = true;
		this.spinbutton1.Value = 8;
		this.table1.Add (this.spinbutton1);
		global::Gtk.Table.TableChild w33 = ((global::Gtk.Table.TableChild)(this.table1 [this.spinbutton1]));
		w33.TopAttach = ((uint)(1));
		w33.BottomAttach = ((uint)(2));
		w33.LeftAttach = ((uint)(2));
		w33.RightAttach = ((uint)(3));
		w33.XOptions = ((global::Gtk.AttachOptions)(4));
		w33.YOptions = ((global::Gtk.AttachOptions)(4));
		this.GtkAlignment.Add (this.table1);
		this.frame1.Add (this.GtkAlignment);
		this.GtkLabel7 = new global::Gtk.Label ();
		this.GtkLabel7.Name = "GtkLabel7";
		this.GtkLabel7.LabelProp = global::Mono.Unix.Catalog.GetString ("<b>Transmit</b>");
		this.GtkLabel7.UseMarkup = true;
		this.frame1.LabelWidget = this.GtkLabel7;
		this.vbox1.Add (this.frame1);
		global::Gtk.Box.BoxChild w36 = ((global::Gtk.Box.BoxChild)(this.vbox1 [this.frame1]));
		w36.Position = 2;
		w36.Expand = false;
		w36.Fill = false;
		// Container child vbox1.Gtk.Box+BoxChild
		this.statusbar2 = new global::Gtk.Statusbar ();
		this.statusbar2.Name = "statusbar2";
		this.statusbar2.Spacing = 6;
		this.vbox1.Add (this.statusbar2);
		global::Gtk.Box.BoxChild w37 = ((global::Gtk.Box.BoxChild)(this.vbox1 [this.statusbar2]));
		w37.Position = 3;
		w37.Expand = false;
		w37.Fill = false;
		this.Add (this.vbox1);
		if ((this.Child != null)) {
			this.Child.ShowAll ();
		}
		this.DefaultWidth = 695;
		this.DefaultHeight = 509;
		this.Show ();
		this.button8.Clicked += new global::System.EventHandler (this.OnButton8Clicked);
		this.button7.Clicked += new global::System.EventHandler (this.OnButton7Clicked);
		this.button6.Clicked += new global::System.EventHandler (this.OnButton6Clicked);
		this.button1.Clicked += new global::System.EventHandler (this.OnButton1Clicked);
	}
}