import bpy

#blenderに登録するアドオン情報
bl_info = {
        "name": "レベルエディタ",
        "author": "Aoto Mori",
        "version": (1, 0),
        "blender": (5, 0, 0),
        "location": "",
        "description": "レベルエディタ",
        "warning": "",
        "wiki_url": "",
        "tracker_url": "",
        "category": "Object",
    }
    
#アドオン有効化時のコールバック
def register():
    print("レベルエディタが有効化されました。")
        
#アドオン無効化時のコールバック
def unregister():
    print("レベルエディタが無効化されました。")

#テスト実行用コード
if __name__ == "__main__":
    register()