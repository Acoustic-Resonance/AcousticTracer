// TODO: App-level state machine / reducer.
//
// Goal:
// Keep `App.jsx` small by moving state transitions and events here.
//
// Suggested high-level states:
// - idle (no upload yet)
// - uploading
// - queued/running (polling status)
// - done (ready for playback)
// - error
//
// Suggested events:
// - SUBMIT_UPLOAD
// - UPLOAD_SUCCESS ({ id })
// - UPLOAD_ERROR ({ error })
// - STATUS_UPDATE ({ status, progress })
// - STATUS_DONE
// - RESET
//
// TODO: Decide whether to implement as:
// - React `useReducer` reducer + action creators, OR
// - a small explicit state machine helper.
//
// TODO: Define initial state shape and action constants.
