package
{
	public interface IMenu
	{
		function get f4se():Object;
		function set f4se(value:Object):void;
		function get BGSCodeObj():Object;
		function set BGSCodeObj(value:Object):void;

		function onCodeObjCreate():void;
		function onCodeObjDestruction():void;
	}
}