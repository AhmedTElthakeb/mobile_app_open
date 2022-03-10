import 'package:flutter/foundation.dart' show ChangeNotifier;

import 'package:shared_preferences/shared_preferences.dart';

class Store extends ChangeNotifier {
  final SharedPreferences _storeFromDisk;

  Store._(this._storeFromDisk);

  static Future<Store> create() async {
    final store = await SharedPreferences.getInstance();
    return Store._(store);
  }

  bool _getBool(String key) {
    final value = _storeFromDisk.getBool(key);
    return value ?? false;
  }

  int _getInt(String key) {
    final value = _storeFromDisk.getInt(key);
    return value ?? 0;
  }

  String _getString(String key) {
    final value = _storeFromDisk.getString(key);
    return value ?? '';
  }

  bool get share => _getBool(_StoreConstants.share);

  set share(bool shareFlag) {
    _storeFromDisk.setBool(_StoreConstants.share, shareFlag);
    notifyListeners();
  }

  bool get submissionMode => _getBool(_StoreConstants.submission_mode);

  set submissionMode(bool submissionModeFlag) {
    _storeFromDisk.setBool(_StoreConstants.submission_mode, submissionModeFlag);
    notifyListeners();
  }

  bool get offlineMode => _getBool(_StoreConstants.offline_mode);

  set offlineMode(bool offlineModeFlag) {
    _storeFromDisk.setBool(_StoreConstants.offline_mode, offlineModeFlag);
    notifyListeners();
  }

  bool get testMode => _getBool(_StoreConstants.test_mode);

  set testMode(bool testModeFlag) {
    _storeFromDisk.setBool(_StoreConstants.test_mode, testModeFlag);
    notifyListeners();
  }

  bool get cooldown => _getBool(_StoreConstants.cooldown);

  set cooldown(bool submissionModeFlag) {
    _storeFromDisk.setBool(_StoreConstants.cooldown, submissionModeFlag);
    notifyListeners();
  }

  int get cooldownPause => _getInt(_StoreConstants.cooldown_pause);

  set cooldownPause(int value) {
    _storeFromDisk.setInt(_StoreConstants.cooldown_pause, value);
    notifyListeners();
  }

  String get chosenConfigurationName =>
      _getString(_StoreConstants.chosen_configuration_name);

  set chosenConfigurationName(String configurationName) {
    _storeFromDisk.setString(
        _StoreConstants.chosen_configuration_name, configurationName);
    notifyListeners();
  }

  String get previousResult => _getString(_StoreConstants.previous_result);

  set previousResult(String result) {
    _storeFromDisk.setString(_StoreConstants.previous_result, result);
  }

  Future<void> deletePreviousResult() async {
    await _storeFromDisk.remove(_StoreConstants.previous_result);
  }

  String get previousExtendedResult =>
      _getString(_StoreConstants.previous_extended_result);

  set previousExtendedResult(String result) {
    _storeFromDisk.setString(_StoreConstants.previous_extended_result, result);
  }

  bool isShareOptionChosen() =>
      _storeFromDisk.containsKey(_StoreConstants.share);

  String get previousAppVersion =>
      _getString(_StoreConstants.previous_app_version);

  set previousAppVersion(String value) {
    _storeFromDisk.setString(_StoreConstants.previous_app_version, value);
  }
}

class _StoreConstants {
  static const share = 'share';
  static const submission_mode = 'submission mode';
  static const offline_mode = 'offline mode';
  static const test_mode = 'test mode';
  static const cooldown = 'cooldown';
  static const cooldown_pause = 'cooldown pause';
  static const chosen_configuration_name = 'chosen configuration name';
  static const previous_result = 'previous result';
  static const previous_extended_result = 'previous extended result';
  static const previous_app_version = 'previous app version';
}