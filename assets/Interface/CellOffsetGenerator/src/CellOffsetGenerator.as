package
{
	import flash.display.*;
	import flash.events.*;
	import flash.text.*;

	import scaleform.gfx.*;

	public final class CellOffsetGenerator extends MovieClip implements IMenu
	{
		private var _f4se:Object;
		private var _bgsCodeObj:Object;

		private var _progressBarBackground:Shape;
		private var _progressBar:Shape;
		private var _progressBarBackgroundBorder:Shape;
		private var _progressBarBorder:Shape;
		private var _progressBarBodyText:TextField;

		private var _tempProgressBarX:Number;
		private var _tempProgressBarY:Number;
		private var _tempProgressBarWidth:Number;
		private var _tempProgressBarHeight:Number;

		private var _tempCurrentProgressCounter:uint;
		private var _tempMaxProgressCounter:uint;

		public function CellOffsetGenerator()
		{
			trace("CellOffsetGenerator.Constructor");

			if (stage == null)
			{
				addEventListener(Event.ADDED_TO_STAGE, onAddedToStage, false, 0, true);
				return;
			}

			setup();
		}

		public function get f4se():Object
		{
			return _f4se;
		}

		public function set f4se(value:Object):void
		{
			_f4se = value;
		}

		public function get BGSCodeObj():Object
		{
			return _bgsCodeObj;
		}

		public function set BGSCodeObj(value:Object):void
		{
			_bgsCodeObj = value;
		}

		public function get progressBarBackground():Shape
		{
			return _progressBarBackground;
		}

		public function get progressBar():Shape
		{
			return _progressBar;
		}

		public function get progressBarBackgroundBorder():Shape
		{
			return _progressBarBackgroundBorder;
		}

		public function get progressBarBorder():Shape
		{
			return _progressBarBorder;
		}

		public function get progressBarBodyText():TextField
		{
			return _progressBarBodyText;
		}

		public function getCurrentProgressCounter():uint
		{
			var getCurrentProgressCounterFunction:Function = (_bgsCodeObj != null) ? (_bgsCodeObj["getCurrentProgressCounter"] as Function) : null;
			return (getCurrentProgressCounterFunction != null) ? getCurrentProgressCounterFunction() : 0;
		}

		public function getMaxProgressCounter():uint
		{
			var getMaxProgressCounterFunction:Function = (_bgsCodeObj != null) ? (_bgsCodeObj["getMaxProgressCounter"] as Function) : null;
			return (getMaxProgressCounterFunction != null) ? getMaxProgressCounterFunction() : 0;
		}

		public function onCodeObjCreate():void
		{
			trace("CellOffsetGenerator.onCodeObjCreate");

			addEventListener(Event.ENTER_FRAME, onEnterFrame, false, 0, true);
		}

		public function onCodeObjDestruction():void
		{
			trace("CellOffsetGenerator.onCodeObjDestruction");

			removeEventListener(Event.ENTER_FRAME, onEnterFrame);
		}

		private function onAddedToStage(event:Event):void
		{
			trace("CellOffsetGenerator.onAddedToStage: " + event);

			removeEventListener(Event.ADDED_TO_STAGE, onAddedToStage);

			setup();
		}

		private function setup():void
		{
			trace("CellOffsetGenerator.setup");

			Extensions.enabled = true;

			_f4se = new Object();
			_bgsCodeObj = new Object();

			var stageWidth:Number = stage.stageWidth;
			var stageHeight:Number = stage.stageHeight;

			_tempProgressBarWidth = stageWidth * Utility.RELATIVE_WIDTH;
			_tempProgressBarHeight = stageHeight * Utility.RELATIVE_HEIGHT;
			_tempProgressBarX = (stageWidth - _tempProgressBarWidth) / 2;
			_tempProgressBarY = _tempProgressBarHeight / 2;

			setupProgressBarBackground();
			setupProgressBar();
			setupProgressBarBackgroundBorder();
			setupProgressBarBorder();
			setupProgressBarBodyText();
		}

		private function setupProgressBarBackground():void
		{
			trace("CellOffsetGenerator.setupProgressBarBackground");

			_progressBarBackground = Utility.createProgressBar(_tempProgressBarX, _tempProgressBarY);

			Utility.drawProgressBar(_progressBarBackground.graphics, 0, 0, _tempProgressBarWidth, _tempProgressBarHeight,
					Utility.DARK_COLOR, Utility.OPAQUE_ALPHA);

			addChild(_progressBarBackground);
		}

		private function setupProgressBar():void
		{
			trace("CellOffsetGenerator.setupProgressBar");

			_progressBar = Utility.createProgressBar(_tempProgressBarX, _tempProgressBarY);

			updateProgressBar(0, 0);

			addChild(_progressBar);
		}

		private function setupProgressBarBackgroundBorder():void
		{
			trace("CellOffsetGenerator.setupProgressBarBackgroundBorder");

			_progressBarBackgroundBorder = Utility.createBorder(_tempProgressBarX, _tempProgressBarY, _tempProgressBarWidth, _tempProgressBarHeight);

			addChild(_progressBarBackgroundBorder);
		}

		private function setupProgressBarBorder():void
		{
			trace("CellOffsetGenerator.setupProgressBarBorder");

			_progressBarBorder = Utility.createBorder(_tempProgressBarX, _tempProgressBarY, _tempProgressBarWidth, _tempProgressBarHeight);

			addChild(_progressBarBorder);
		}

		private function setupProgressBarBodyText():void
		{
			trace("CellOffsetGenerator.setupProgressBarBodyText");

			var textFieldWidth:Number = _tempProgressBarWidth;
			var textFieldHeight:Number = _tempProgressBarHeight;
			var textFieldX:Number = _tempProgressBarX;
			var textFieldY:Number = _tempProgressBarY + textFieldHeight;

			_progressBarBodyText = Utility.createTextField(textFieldX, textFieldY, textFieldWidth, textFieldHeight,
					"", TextFormatAlign.CENTER, TextFieldAutoSize.CENTER, TextFieldEx.VALIGN_CENTER, TextFieldEx.VAUTOSIZE_TOP);

			updateProgressBarBodyText(0, 0);

			addChild(_progressBarBodyText);
		}

		private function onEnterFrame(event:Event):void
		{
			trace("CellOffsetGenerator.onEnterFrame: " + event);

			var currentProgress:uint = getCurrentProgressCounter();
			var maxProgress:uint = getMaxProgressCounter();

			if (currentProgress == _tempCurrentProgressCounter && maxProgress == _tempMaxProgressCounter)
			{
				return;
			}

			_tempCurrentProgressCounter = currentProgress;
			_tempMaxProgressCounter = maxProgress;

			updateProgressBar(currentProgress, maxProgress);
			updateProgressBarBodyText(currentProgress, maxProgress);
		}

		public function updateProgressBar(currentProgress:uint, maxProgress:uint):void
		{
			trace("CellOffsetGenerator.updateProgressBar: " + currentProgress + "/" + maxProgress);

			var percentProgress:Number = (maxProgress > 0) ? currentProgress / maxProgress : 0;

			Utility.drawProgressBar(_progressBar.graphics, 0, 0, _tempProgressBarWidth * percentProgress, _tempProgressBarHeight,
					Utility.LIGHT_COLOR, Utility.FULL_ALPHA);
		}

		public function updateProgressBarBodyText(currentProgress:uint, maxProgress:uint):void
		{
			trace("CellOffsetGenerator.updateProgressBarBodyText: " + currentProgress + "/" + maxProgress);

			var bodyText:String = Utility.formatBodyText(currentProgress, maxProgress);
			_progressBarBodyText.text = bodyText;
		}
	}
}