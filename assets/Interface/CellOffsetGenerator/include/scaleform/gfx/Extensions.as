package scaleform.gfx
{
	import flash.display.*;
	import flash.geom.*;
	import flash.text.*;

	public final class Extensions
	{
		public static const EDGEAA_INHERIT:uint = 0;
		public static const EDGEAA_ON:uint = 1;
		public static const EDGEAA_OFF:uint = 2;
		public static const EDGEAA_DISABLE:uint = 3;

		public static var isGFxPlayer:Boolean;

		public static var CLIK_addedToStageCallback:Function;
		public static var gfxProcessSound:Function;

		public native function Extensions();

		public static native function get isScaleform():Boolean;

		public static native function get enabled():Boolean;

		public static native function set enabled(value:Boolean):void;

		public static native function get numControllers():uint;

		public static native function get visibleRect():Rectangle;

		public static native function get noInvisibleAdvance():Boolean;

		public static native function set noInvisibleAdvance(value:Boolean):void;

		public static native function getTopMostEntity(x:Number, y:Number, testAll:Boolean = true):DisplayObject;

		public static native function getMouseTopMostEntity(testAll:Boolean = true, mouseIndex:uint = 0):DisplayObject;

		public static native function setMouseCursorType(cursor:String, mouseIndex:uint = 0):void;

		public static native function getMouseCursorType(mouseIndex:uint = 0):String;

		public static native function getEdgeAAMode(dispObj:DisplayObject):uint;

		public static native function setEdgeAAMode(dispObj:DisplayObject, mode:uint):void;

		public static native function setIMEEnabled(textField:TextField, isEnabled:Boolean):void;
	}
}
