package
{
	import flash.display.*;
	import flash.filters.*;
	import flash.text.*;

	import scaleform.gfx.*;

	public final class Utility
	{
		private static const TEXT_FIELD_DUMMY:TextField = new TextField();

		public static const MAIN_FONT:String = "$MAIN_Font";
		public static const MAIN_FONT_BOLD:String = "$MAIN_Font_Bold";

		public static const BODY_TEXT:String = translateText("$CellOffsetGenerator_BodyText");
		public static const BODY_TEXT_INITIALIZING:String = translateText("$CellOffsetGenerator_BodyText_Initializing");

		public static const RELATIVE_WIDTH:Number = 0.3;
		public static const RELATIVE_HEIGHT:Number = 0.03;

		public static const LIGHT_COLOR:uint = 0xEEEEEE;
		public static const DARK_COLOR:uint = 0x333333;

		public static const OPAQUE_ALPHA:Number = 0.75;
		public static const FULL_ALPHA:Number = 1.0;

		public static const ELLIPSE_SIZE:Number = 2.5;
		public static const TEXT_SIZE:uint = 18;
		public static const BORDER_SIZE:Number = 3.0;

		public static const SMALL_SHADOW_SIZE:Number = 1.0;
		public static const LARGE_SHADOW_SIZE:Number = 2.0;

		public static const SHADOW_BLUR_X:Number = 0.25;
		public static const SHADOW_BLUR_Y:Number = 0.25;

		public static function createProgressBar(x:Number, y:Number):Shape
		{
			var progressBarFilter:DropShadowFilter = new DropShadowFilter();
			progressBarFilter.distance = LARGE_SHADOW_SIZE;
			progressBarFilter.blurX = SHADOW_BLUR_X;
			progressBarFilter.blurY = SHADOW_BLUR_Y;

			var progressBar:Shape = new Shape();
			progressBar.x = x;
			progressBar.y = y;
			progressBar.filters = [progressBarFilter];

			return progressBar;
		}

		public static function createTextField(x:Number, y:Number, width:Number, height:Number,
				text:String, align:String, autoSize:String, verticalAlign:String, verticalAutoSize:String):TextField
		{
			var textFormat:TextFormat = new TextFormat();
			textFormat.font = MAIN_FONT_BOLD;
			textFormat.size = TEXT_SIZE;
			textFormat.color = LIGHT_COLOR;
			textFormat.align = align;

			var textShadow:DropShadowFilter = new DropShadowFilter();
			textShadow.distance = SMALL_SHADOW_SIZE;
			textShadow.blurX = SHADOW_BLUR_X;
			textShadow.blurY = SHADOW_BLUR_Y;

			var textField:TextField = new TextField();
			textField.x = x;
			textField.y = y;
			textField.width = width;
			textField.height = height;

			textField.defaultTextFormat = textFormat;
			textField.text = "";
			textField.text = text;
			textField.autoSize = autoSize;
			textField.filters = [textShadow];

			textField.selectable = false;
			textField.mouseEnabled = false;

			TextFieldEx.setVerticalAlign(textField, verticalAlign);
			TextFieldEx.setVerticalAutoSize(textField, verticalAutoSize);

			return textField;
		}

		public static function createBorder(x:Number, y:Number, width:Number, height:Number):Shape
		{
			var textShadow:DropShadowFilter = new DropShadowFilter();
			textShadow.distance = LARGE_SHADOW_SIZE;
			textShadow.blurX = SHADOW_BLUR_X;
			textShadow.blurY = SHADOW_BLUR_Y;

			var border:Shape = new Shape();
			border.x = x;
			border.y = y;
			border.graphics.lineStyle(BORDER_SIZE, LIGHT_COLOR);
			border.graphics.drawRect(0, 0, width, height);
			border.filters = [textShadow];

			return border;
		}

		public static function drawProgressBar(component:Graphics, x:Number, y:Number, width:Number, height:Number,
				backgroundColor:uint, backgroundAlpha:Number):void
		{
			if (component == null)
			{
				return;
			}

			component.clear();
			component.beginFill(backgroundColor, backgroundAlpha);
			component.drawRoundRect(x, y, width, height, ELLIPSE_SIZE, ELLIPSE_SIZE);
			component.endFill();
		}

		public static function formatString(format:String, ...args:Array):String
		{
			if (!format || !args)
			{
				return format;
			}

			var result:String = format;

			for (var i:uint = 0; i < args.length; i++)
			{
				result = result.replace("{" + i + "}", args[i]);
			}

			return result;
		}

		public static function formatBodyText(currentProgress:uint, maxProgress:uint):String
		{
			if (maxProgress == 0)
			{
				return BODY_TEXT_INITIALIZING;
			}

			var percentProgress:uint = (maxProgress > 0) ? Math.floor(((currentProgress as Number) / (maxProgress as Number)) * 100.0) : 0;
			return formatString(BODY_TEXT, percentProgress, currentProgress, maxProgress);
		}

		public static function translateText(key:String):String
		{
			if (!key)
			{
				return "";
			}

			TEXT_FIELD_DUMMY.text = key;
			var result:String = TEXT_FIELD_DUMMY.text;
			TEXT_FIELD_DUMMY.text = "";
			return result;
		}
	}
}